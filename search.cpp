#include "search.h"
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <cstring>

static uint64_t rand64() {
    static uint64_t seed = 0x123456789ABCDEF0ULL;
    seed ^= seed << 13;
    seed ^= seed >> 7;
    seed ^= seed << 17;
    return seed;
}

Search::Search() {
    tt = std::make_unique<TTEntry[]>(TT_SIZE);
    memset(tt.get(), 0, sizeof(TTEntry) * TT_SIZE);
    initZobrist();
}

void Search::initZobrist() {
    for (int c = 0; c < 2; c++)
        for (int pt = 0; pt < 6; pt++)
            for (int sq = 0; sq < 64; sq++)
                zobristPiece[c][pt][sq] = rand64();
    zobristSide = rand64();
    for (int i = 0; i < 16; i++) zobristCastle[i] = rand64();
    for (int i = 0; i < 8; i++) zobristEP[i] = rand64();
}

uint64_t Search::computeHash(const Position& pos) const {
    uint64_t h = 0;
    for (int c = 0; c < 2; c++)
        for (int pt = 0; pt < 6; pt++) {
            Bitboard bb = pos.pieces[c][pt];
            while (bb) {
                Square sq = popLsb(bb);
                h ^= zobristPiece[c][pt][sq];
            }
        }
    if (pos.sideToMove == Colour::Black) h ^= zobristSide;
    h ^= zobristCastle[pos.castlingRights & 0xF];
    if (pos.enPassantSquare != NO_SQUARE)
        h ^= zobristEP[fileOf(pos.enPassantSquare)];
    return h;
}

void Search::storeTT(uint64_t hash, int score, int depth,
    Move move, TTFlag flag) {
    TTEntry& e = tt[hash & (TT_SIZE - 1)];
    e = { hash, score, depth, move, flag };
}

bool Search::probeTT(uint64_t hash, int depth, int alpha,
    int beta, int& score, Move& move) const {
    const TTEntry& e = tt[hash & (TT_SIZE - 1)];
    if (e.hash != hash) return false;

    move = e.move;
    if (e.depth < depth) return false; 

    if (e.flag == TT_EXACT) { score = e.score; return true; }
    if (e.flag == TT_LOWER && e.score >= beta) { score = e.score; return true; }
    if (e.flag == TT_UPPER && e.score <= alpha) { score = e.score; return true; }
    return false;
}

static const int MVV_LVA[6][6] = {
    { 105, 104, 103, 102, 101, 100 },
    { 205, 204, 203, 202, 201, 200 },
    { 305, 304, 303, 302, 301, 300 },
    { 405, 404, 403, 402, 401, 400 },
    { 505, 504, 503, 502, 501, 500 },
    {   0,   0,   0,   0,   0,   0 },
};

int Search::scoreMoveForOrdering(const Position& pos, Move m, Move ttMove) const {
    if (m == ttMove)    return 10000;
    if (m.isCastling()) return 60;

    Piece moving = pos.pieceOn(m.from());
    Piece captured = pos.pieceOn(m.to());

    if (!captured.isEmpty()) {
        int attacker = static_cast<int>(moving.type);
        int victim = static_cast<int>(captured.type);
        return MVV_LVA[victim][attacker] + 1000;
    }
    if (m.isPromotion()) return 900;
    return 0;
}

void Search::orderMoves(const Position& pos, MoveList& list,
    Move ttMove) {
    int scores[256];
    for (int i = 0; i < list.count; i++)
        scores[i] = scoreMoveForOrdering(pos, list.moves[i], ttMove);

    for (int i = 1; i < list.count; i++) {
        Move key = list.moves[i];
        int  keyS = scores[i];
        int  j = i - 1;
        while (j >= 0 && scores[j] < keyS) {
            list.moves[j + 1] = list.moves[j];
            scores[j + 1] = scores[j];
            j--;
        }
        list.moves[j + 1] = key;
        scores[j + 1] = keyS;
    }
}

int Search::quiescence(Position& pos, int alpha, int beta,
    uint64_t hash) {
    if (stop) return 0;
    nodes++;

    int standPat = evaluate(pos);
    if (standPat >= beta)  return beta;
    if (standPat > alpha)  alpha = standPat;

    MoveList list;
    generateCaptures(pos, list);
    orderMoves(pos, list, NULL_MOVE);

    Colour us = pos.sideToMove;

    for (int i = 0; i < list.count; i++) {
        Move    m = list.moves[i];
        Piece   moving = pos.pieceOn(m.from());
        uint8_t prevC = pos.castlingRights;
        Square  prevE = pos.enPassantSquare;
        int     prevH = pos.halfMoveClock;

        Piece captured = pos.makeMove(m);
        if (pos.isInCheck(us)) {
            pos.unmakeMove(m, moving, captured, prevC, prevE, prevH);
            continue;
        }

        int score = -quiescence(pos, -beta, -alpha, hash);
        pos.unmakeMove(m, moving, captured, prevC, prevE, prevH);

        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }
    return alpha;
}

int Search::negamax(Position& pos, int depth, int alpha, int beta,
    uint64_t hash) {
    if (stop) return 0;
    nodes++;

    if (depth == 0)
        return quiescence(pos, alpha, beta, hash);

    int  ttScore = 0;
    Move ttMove = NULL_MOVE;
    if (probeTT(hash, depth, alpha, beta, ttScore, ttMove))
        return ttScore;

    MoveList list;
    generateMoves(pos, list);
    orderMoves(pos, list, ttMove);

    Colour us = pos.sideToMove;
    bool anyLegal = false;
    int best = -INF_SCORE;
    Move bestMove = NULL_MOVE;
    int origAlpha = alpha;

    for (int i = 0; i < list.count; i++) {
        Move m = list.moves[i];
        Piece moving = pos.pieceOn(m.from());
        uint8_t prevC = pos.castlingRights;
        Square prevE = pos.enPassantSquare;
        int prevH = pos.halfMoveClock;

        Piece captured = pos.makeMove(m);

        if (pos.isInCheck(us)) {
            pos.unmakeMove(m, moving, captured, prevC, prevE, prevH);
            continue;
        }

        anyLegal = true;
        uint64_t newHash = computeHash(pos);
        int score = -negamax(pos, depth - 1, -beta, -alpha, newHash);
        pos.unmakeMove(m, moving, captured, prevC, prevE, prevH);

        if (score > best) {
            best = score;
            bestMove = m;
        }
        if (score > alpha) alpha = score;
        if (alpha >= beta) break;
    }

    if (!anyLegal) {
        return pos.isInCheck(us) ? (-MATE_SCORE + depth) : 0;
    }

    TTFlag flag = (best <= origAlpha) ? TT_UPPER
        : (best >= beta) ? TT_LOWER
        : TT_EXACT;
    storeTT(hash, best, depth, bestMove, flag);

    return best;
}

SearchResult Search::searchRoot(Position& pos, int depth) {
    MoveList list;
    generateMoves(pos, list);

    Colour us = pos.sideToMove;
    int    best = -INF_SCORE;
    Move   bestM = NULL_MOVE;

    Move ttMove = NULL_MOVE;
    int  dummy = 0;
    probeTT(computeHash(pos), depth, -INF_SCORE, INF_SCORE, dummy, ttMove);
    orderMoves(pos, list, ttMove);

    for (int i = 0; i < list.count; i++) {
        Move    m = list.moves[i];
        Piece   moving = pos.pieceOn(m.from());
        uint8_t prevC = pos.castlingRights;
        Square  prevE = pos.enPassantSquare;
        int     prevH = pos.halfMoveClock;

        Piece captured = pos.makeMove(m);
        if (pos.isInCheck(us)) {
            pos.unmakeMove(m, moving, captured, prevC, prevE, prevH);
            continue;
        }

        uint64_t newHash = computeHash(pos);
        int score = -negamax(pos, depth - 1,
            -INF_SCORE, INF_SCORE, newHash);
        pos.unmakeMove(m, moving, captured, prevC, prevE, prevH);

        if (score > best) {
            best = score;
            bestM = m;
        }
    }

    return { bestM, best, depth, nodes };
}

SearchResult Search::findBestMove(Position& pos, int maxDepth) {
    nodes = 0;
    SearchResult result;

    for (int depth = 1; depth <= maxDepth; depth++) {
        SearchResult r = searchRoot(pos, depth);
        result = r;
        std::cout << "[Search] depth=" << depth
            << " score=" << result.score
            << " nodes=" << nodes << "\n";
        if (stop) break;
    }

    return result;
}