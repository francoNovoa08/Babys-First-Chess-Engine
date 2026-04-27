#include "movegen.h"
#include "bitboard.h"

static Bitboard knightAttacks[64];
static Bitboard kingAttacks[64];
static bool tablesInitialised = false;

static void initialiseTables() {
    if (tablesInitialised) return;

    const int knightDF[] = { 1, 2,  2,  1, -1, -2, -2, -1 };
    const int knightDR[] = { 2, 1, -1, -2, -2, -1,  1,  2 };

    for (int sq = 0; sq < 64; sq++) {
        int f = fileOf(sq), r = rankOf(sq);
        knightAttacks[sq] = 0;
        for (int i = 0; i < 8; i++) {
            int nf = f + knightDF[i];
            int nr = r + knightDR[i];
            if (nf >= 0 && nf < 8 && nr >= 0 && nr < 8)
                knightAttacks[sq] |= squareBB(makeSquare(nf, nr));
        }

        kingAttacks[sq] = 0;
        const int kingDF[] = { 1, 1, 0, -1, -1, -1,  0,  1 };
        const int kingDR[] = { 0, 1, 1,  1,  0, -1, -1, -1 };
        for (int i = 0; i < 8; i++) {
            int nf = f + kingDF[i];
            int nr = r + kingDR[i];
            if (nf >= 0 && nf < 8 && nr >= 0 && nr < 8)
                kingAttacks[sq] |= squareBB(makeSquare(nf, nr));
        }
    }
    tablesInitialised = true;
}

static void addPromotions(MoveList& list, Square from, Square to) {
    list.add(Move::makePromotion(from, to, PieceType::Queen));
    list.add(Move::makePromotion(from, to, PieceType::Rook));
    list.add(Move::makePromotion(from, to, PieceType::Bishop));
    list.add(Move::makePromotion(from, to, PieceType::Knight));
}

static void generatePawnMoves(const Position& pos, MoveList& list,
    bool capturesOnly) {
    Colour us = pos.sideToMove;
    Colour them = opponent(us);
    int    c = static_cast<int>(us);

    Bitboard pawns = pos.getPieces(us, PieceType::Pawn);
    Bitboard empty = ~pos.allOccupied;
    Bitboard enemies = pos.getOccupied(them);

    if (us == Colour::White) {
        Bitboard push1 = shiftN(pawns) & empty;
        Bitboard push2 = shiftN(push1 & RANK_3) & empty;
        Bitboard captureE = shiftNE(pawns) & enemies;
        Bitboard captureW = shiftNW(pawns) & enemies;

        if (!capturesOnly) {
            Bitboard quietPush = push1 & ~RANK_8;
            while (quietPush) {
                Square to = popLsb(quietPush);
                Square from = to - 8;
                list.add(Move::make(from, to));
            }
            while (push2) {
                Square to = popLsb(push2);
                Square from = to - 16;
                list.add(Move::make(from, to));
            }
        }

        Bitboard promoPush = push1 & RANK_8;
        while (promoPush) {
            Square to = popLsb(promoPush);
            Square from = to - 8;
            addPromotions(list, from, to);
        }

        Bitboard promoCaptE = captureE & RANK_8;
        Bitboard promoCaptW = captureW & RANK_8;
        captureE &= ~RANK_8;
        captureW &= ~RANK_8;

        while (captureE) {
            Square to = popLsb(captureE);
            list.add(Move::make(to - 9, to));
        }
        while (captureW) {
            Square to = popLsb(captureW);
            list.add(Move::make(to - 7, to));
        }
        while (promoCaptE) {
            Square to = popLsb(promoCaptE);
            addPromotions(list, to - 9, to);
        }
        while (promoCaptW) {
            Square to = popLsb(promoCaptW);
            addPromotions(list, to - 7, to);
        }

        if (pos.enPassantSquare != NO_SQUARE) {
            Square ep = pos.enPassantSquare;
            Bitboard epBB = squareBB(ep);
            if (shiftNE(pawns) & epBB)
                list.add(Move::makeEnPassant(ep - 9, ep));
            if (shiftNW(pawns) & epBB)
                list.add(Move::makeEnPassant(ep - 7, ep));
        }

    }
    else { 
        Bitboard push1 = shiftS(pawns) & empty;
        Bitboard push2 = shiftS(push1 & RANK_6) & empty;
        Bitboard captureE = shiftSE(pawns) & enemies;
        Bitboard captureW = shiftSW(pawns) & enemies;

        if (!capturesOnly) {
            Bitboard quietPush = push1 & ~RANK_1;
            while (quietPush) {
                Square to = popLsb(quietPush);
                Square from = to + 8;
                list.add(Move::make(from, to));
            }
            while (push2) {
                Square to = popLsb(push2);
                Square from = to + 16;
                list.add(Move::make(from, to));
            }
        }

        Bitboard promoPush = push1 & RANK_1;
        while (promoPush) {
            Square to = popLsb(promoPush);
            Square from = to + 8;
            addPromotions(list, from, to);
        }

        Bitboard promoCaptE = captureE & RANK_1;
        Bitboard promoCaptW = captureW & RANK_1;
        captureE &= ~RANK_1;
        captureW &= ~RANK_1;

        while (captureE) {
            Square to = popLsb(captureE);
            list.add(Move::make(to + 7, to));
        }
        while (captureW) {
            Square to = popLsb(captureW);
            list.add(Move::make(to + 9, to));
        }
        while (promoCaptE) {
            Square to = popLsb(promoCaptE);
            addPromotions(list, to + 7, to);
        }
        while (promoCaptW) {
            Square to = popLsb(promoCaptW);
            addPromotions(list, to + 9, to);
        }

        if (pos.enPassantSquare != NO_SQUARE) {
            Square ep = pos.enPassantSquare;
            Bitboard epBB = squareBB(ep);
            if (shiftSE(pawns) & epBB)
                list.add(Move::makeEnPassant(ep + 7, ep));
            if (shiftSW(pawns) & epBB)
                list.add(Move::makeEnPassant(ep + 9, ep));
        }
    }
}

static void generateKnightMoves(const Position& pos, MoveList& list,
    bool capturesOnly) {
    Colour us = pos.sideToMove;
    Bitboard mine = pos.getOccupied(us);
    Bitboard them = pos.getOccupied(opponent(us));
    Bitboard bb = pos.getPieces(us, PieceType::Knight);

    while (bb) {
        Square from = popLsb(bb);
        Bitboard atks = knightAttacks[from] & ~mine;
        if (capturesOnly) atks &= them;
        while (atks) {
            list.add(Move::make(from, popLsb(atks)));
        }
    }
}

static void generateBishopMoves(const Position& pos, MoveList& list,
    bool capturesOnly) {
    Colour us = pos.sideToMove;
    Bitboard mine = pos.getOccupied(us);
    Bitboard them = pos.getOccupied(opponent(us));
    Bitboard bb = pos.getPieces(us, PieceType::Bishop);

    while (bb) {
        Square from = popLsb(bb);
        Bitboard atks = bishopAttacks(from, pos.allOccupied) & ~mine;
        if (capturesOnly) atks &= them;
        while (atks) {
            list.add(Move::make(from, popLsb(atks)));
        }
    }
}

static void generateRookMoves(const Position& pos, MoveList& list,
    bool capturesOnly) {
    Colour us = pos.sideToMove;
    Bitboard mine = pos.getOccupied(us);
    Bitboard them = pos.getOccupied(opponent(us));
    Bitboard bb = pos.getPieces(us, PieceType::Rook);

    while (bb) {
        Square from = popLsb(bb);
        Bitboard atks = rookAttacks(from, pos.allOccupied) & ~mine;
        if (capturesOnly) atks &= them;
        while (atks) {
            list.add(Move::make(from, popLsb(atks)));
        }
    }
}

static void generateQueenMoves(const Position& pos, MoveList& list,
    bool capturesOnly) {
    Colour us = pos.sideToMove;
    Bitboard mine = pos.getOccupied(us);
    Bitboard them = pos.getOccupied(opponent(us));
    Bitboard bb = pos.getPieces(us, PieceType::Queen);

    while (bb) {
        Square from = popLsb(bb);
        Bitboard atks = queenAttacks(from, pos.allOccupied) & ~mine;
        if (capturesOnly) atks &= them;
        while (atks) {
            list.add(Move::make(from, popLsb(atks)));
        }
    }
}

static void generateKingMoves(const Position& pos, MoveList& list,
    bool capturesOnly) {
    Colour us = pos.sideToMove;
    Bitboard mine = pos.getOccupied(us);
    Bitboard them = pos.getOccupied(opponent(us));
    Bitboard bb = pos.getPieces(us, PieceType::King);
    if (!bb) return;

    Square from = lsb(bb);
    Bitboard atks = kingAttacks[from] & ~mine;
    if (capturesOnly) atks &= them;
    while (atks) {
        list.add(Move::make(from, popLsb(atks)));
    }

    if (capturesOnly) return;

    Colour them2 = opponent(us);
    if (us == Colour::White) {
        // Kingside
        if ((pos.castlingRights & WHITE_OO) &&
            !(pos.allOccupied & (squareBB(5) | squareBB(6))) &&
            !pos.isSquareAttacked(4, them2) &&
            !pos.isSquareAttacked(5, them2) &&
            !pos.isSquareAttacked(6, them2)) {
            list.add(Move::makeCastle(4, 6));
        }
        // Queenside
        if ((pos.castlingRights & WHITE_OOO) &&
            !(pos.allOccupied & (squareBB(1) | squareBB(2) | squareBB(3))) &&
            !pos.isSquareAttacked(4, them2) &&
            !pos.isSquareAttacked(3, them2) &&
            !pos.isSquareAttacked(2, them2)) {
            list.add(Move::makeCastle(4, 2));
        }
    }
    else {
        // Kingside
        if ((pos.castlingRights & BLACK_OO) &&
            !(pos.allOccupied & (squareBB(61) | squareBB(62))) &&
            !pos.isSquareAttacked(60, them2) &&
            !pos.isSquareAttacked(61, them2) &&
            !pos.isSquareAttacked(62, them2)) {
            list.add(Move::makeCastle(60, 62));
        }
        // Queenside
        if ((pos.castlingRights & BLACK_OOO) &&
            !(pos.allOccupied & (squareBB(57) | squareBB(58) | squareBB(59))) &&
            !pos.isSquareAttacked(60, them2) &&
            !pos.isSquareAttacked(59, them2) &&
            !pos.isSquareAttacked(58, them2)) {
            list.add(Move::makeCastle(60, 58));
        }
    }
}


void generateMoves(const Position& pos, MoveList& list) {
    initialiseTables();
    generatePawnMoves(pos, list, false);
    generateKnightMoves(pos, list, false);
    generateBishopMoves(pos, list, false);
    generateRookMoves(pos, list, false);
    generateQueenMoves(pos, list, false);
    generateKingMoves(pos, list, false);
}

void generateCaptures(const Position& pos, MoveList& list) {
    initialiseTables();
    generatePawnMoves(pos, list, true);
    generateKnightMoves(pos, list, true);
    generateBishopMoves(pos, list, true);
    generateRookMoves(pos, list, true);
    generateQueenMoves(pos, list, true);
    generateKingMoves(pos, list, true);
}

bool hasLegalMove(Position& pos) {
    initialiseTables();
    MoveList list;
    generateMoves(pos, list);
    Colour us = pos.sideToMove;

    for (int i = 0; i < list.count; i++) {
        Move m = list.moves[i];
        Piece moving = pos.pieceOn(m.from());
        uint8_t prevC = pos.castlingRights;
        Square  prevE = pos.enPassantSquare;
        int     prevH = pos.halfMoveClock;

        Piece captured = pos.makeMove(m);
        bool inCheck = pos.isInCheck(us);
        pos.unmakeMove(m, moving, captured, prevC, prevE, prevH);

        if (!inCheck) return true;
    }
    return false;
}