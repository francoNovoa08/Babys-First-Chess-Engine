#include "uci.h"
#include "movegen.h"
#include "helpers.h"
#include <iostream>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <cctype>

std::string squareToStr(Square sq) {
    if (sq == NO_SQUARE) return "-";
    std::string s;
    s += static_cast<char>('a' + fileOf(sq));
    s += static_cast<char>('1' + rankOf(sq));
    return s;
}

Square strToSquare(const std::string& s) {
    if (s.size() < 2) return NO_SQUARE;
    int file = s[0] - 'a';
    int rank = s[1] - '1';
    if (file < 0 || file > 7 || rank < 0 || rank > 7) return NO_SQUARE;
    return makeSquare(file, rank);
}

std::string moveToStr(Move m) {
    if (m.isNull()) return "0000";
    std::string s = squareToStr(m.from()) + squareToStr(m.to());
    if (m.isPromotion()) {
        constexpr char promoChars[] = { 'n', 'b', 'r', 'q' };
        int idx = static_cast<int>(m.promo()) - 1;
        if (idx >= 0 && idx < 4) s += promoChars[idx];
    }
    return s;
}

Move strToMove(const std::string& s, const Position& pos) {
    if (s.size() < 4) return NULL_MOVE;

    Square from = strToSquare(s.substr(0, 2));
    Square to = strToSquare(s.substr(2, 2));
    if (from == NO_SQUARE || to == NO_SQUARE) return NULL_MOVE;

    MoveList list;
    generateMoves(pos, list);

    for (int i = 0; i < list.count; i++) {
        Move m = list.moves[i];
        if (m.from() != from || m.to() != to) continue;

        if (m.isPromotion()) {
            if (s.size() < 5) continue;
            char pc = static_cast<char>(std::tolower(static_cast<unsigned char>(s[4])));
            PieceType expected;
            switch (pc) {
            case 'n': expected = PieceType::Knight; break;
            case 'b': expected = PieceType::Bishop; break;
            case 'r': expected = PieceType::Rook;   break;
            case 'q': expected = PieceType::Queen;  break;
            default:  continue;
            }
            if (m.promo() != expected) continue;
        }
        return m;
    }
    return NULL_MOVE;
}

static Piece fenCharToPiece(char c) {
    Colour col = std::isupper(static_cast<unsigned char>(c))
        ? Colour::White : Colour::Black;
    switch (std::tolower(static_cast<unsigned char>(c))) {
    case 'p': return { PieceType::Pawn,   col };
    case 'n': return { PieceType::Knight, col };
    case 'b': return { PieceType::Bishop, col };
    case 'r': return { PieceType::Rook,   col };
    case 'q': return { PieceType::Queen,  col };
    case 'k': return { PieceType::King,   col };
    default:  return { PieceType::None,   Colour::None };
    }
}

static void parseFen(Position& pos, const std::string& fen) {
    pos.clear();
    std::istringstream ss(fen);
    std::string board, side, castling, ep, halfMove, fullMove;
    ss >> board >> side >> castling >> ep >> halfMove >> fullMove;

    int rank = 7, file = 0;
    for (char c : board) {
        if (c == '/') {
            rank--;
            file = 0;
        }
        else if (std::isdigit(static_cast<unsigned char>(c))) {
            file += c - '0';
        }
        else {
            Piece p = fenCharToPiece(c);
            if (!p.isEmpty())
                pos.putPiece(p, makeSquare(file, rank));
            file++;
        }
    }

    pos.sideToMove = (side == "b") ? Colour::Black : Colour::White;

    pos.castlingRights = 0;
    if (castling != "-") {
        for (char c : castling) {
            switch (c) {
            case 'K': pos.castlingRights |= WHITE_OO;  break;
            case 'Q': pos.castlingRights |= WHITE_OOO; break;
            case 'k': pos.castlingRights |= BLACK_OO;  break;
            case 'q': pos.castlingRights |= BLACK_OOO; break;
            }
        }
    }

    pos.enPassantSquare = (ep != "-") ? strToSquare(ep) : NO_SQUARE;

    try {
        pos.halfMoveClock = halfMove.empty() ? 0 : std::stoi(halfMove);
        pos.fullMoveNumber = fullMove.empty() ? 1 : std::stoi(fullMove);
    }
    catch (...) {
        pos.halfMoveClock = 0;
        pos.fullMoveNumber = 1;
    }

    pos.updateOccupancy();
}

struct TimeControl {
    int wtime = 0;  
    int btime = 0;
    int winc = 0;   
    int binc = 0;
    int movestogo = 0;
    int movetime = 0;
    bool infinite = false;
};

static int allocateTime(const TimeControl& tc, Colour side) {
    if (tc.infinite || tc.movetime > 0) return tc.movetime; 

    int remaining = (side == Colour::White) ? tc.wtime : tc.btime;
    int inc = (side == Colour::White) ? tc.winc : tc.binc;

    int movestogo = (tc.movestogo > 0) ? tc.movestogo : 30;
    int allocated = remaining / movestogo + inc * 3 / 4;
    return std::max(allocated, 50);
}

static void parsePosition(Position& pos, std::istringstream& ss) {
    std::string token;
    ss >> token;

    if (token == "startpos") {
        pos.setStartingPosition();
        ss >> token;
    }
    else if (token == "fen") {
        std::string fen;
        while (ss >> token && token != "moves")
            fen += (fen.empty() ? "" : " ") + token;
        parseFen(pos, fen);
        if (token != "moves") return;
    }

    while (ss >> token) {
        Move m = strToMove(token, pos);
        if (m.isNull()) break;

        Piece moving = pos.pieceOn(m.from());
        uint8_t prevC = pos.castlingRights;
        Square  prevE = pos.enPassantSquare;
        int     prevH = pos.halfMoveClock;
        pos.makeMove(m);
        (void)prevC; (void)prevE; (void)prevH; (void)moving;
    }
}

static void handleGo(Position& pos, Search& search,
    std::istringstream& ss) {
    TimeControl tc;
    std::string token;

    while (ss >> token) {
        if (token == "wtime")     ss >> tc.wtime;
        else if (token == "btime")     ss >> tc.btime;
        else if (token == "winc")      ss >> tc.winc;
        else if (token == "binc")      ss >> tc.binc;
        else if (token == "movestogo") ss >> tc.movestogo;
        else if (token == "movetime")  ss >> tc.movetime;
        else if (token == "infinite")  tc.infinite = true;
    }

    int timeAllotted = tc.infinite ? 0 : allocateTime(tc, pos.sideToMove);
    auto deadline = std::chrono::steady_clock::now()
        + std::chrono::milliseconds(timeAllotted);

    SearchResult best;
    search.resetStop();
    constexpr int MAX_DEPTH = 6;

    for (int depth = 1; depth <= MAX_DEPTH; depth++) {
        SearchResult r = search.searchRoot(pos, depth);

        if (!search.isStopped())
            best = r;

        std::cout << "info depth " << depth
            << " score cp " << best.score
            << " nodes " << best.nodes
            << " pv " << moveToStr(best.bestMove)
            << "\n";
        std::cout.flush();

        if (!tc.infinite &&
            std::chrono::steady_clock::now() >= deadline) {
            search.stop = true;
            break;
        }
    }

    std::cout << "bestmove " << moveToStr(best.bestMove) << "\n";
    std::cout.flush();
}

void uciLoop() {
    Position pos;
    pos.setStartingPosition();
    Search search;

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;

        std::istringstream ss(line);
        std::string cmd;
        ss >> cmd;

        if (cmd == "uci") {
            std::cout << "id name BabysFirstChessEngine\n"
                << "id author Franco\n"
                << "uciok\n";
            std::cout.flush();

        }
        else if (cmd == "isready") {
            std::cout << "readyok\n";
            std::cout.flush();

        }
        else if (cmd == "ucinewgame") {
            pos.setStartingPosition();
            search.resetTT();

        }
        else if (cmd == "position") {
            parsePosition(pos, ss);

        }
        else if (cmd == "go") {
            handleGo(pos, search, ss);

        }
        else if (cmd == "stop") {
            search.stop = true;

        }
        else if (cmd == "quit") {
            break;
        }
    }
}