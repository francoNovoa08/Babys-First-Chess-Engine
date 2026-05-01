#include <vector>
#include "position.h"
#include "movegen.h"

std::vector<Square> legalDestinations(Position& pos, Square from) {
    MoveList list;
    generateMoves(pos, list);

    std::vector<Square> dests;
    Colour us = pos.sideToMove;

    for (int i = 0; i < list.count; i++) {
        Move m = list.moves[i];
        if (m.from() != from) continue;

        Piece moving = pos.pieceOn(m.from());
        uint8_t prevC = pos.castlingRights;
        Square prevE = pos.enPassantSquare;
        int prevH = pos.halfMoveClock;

        Piece captured = pos.makeMove(m);
        bool  illegal = pos.isInCheck(us);
        pos.unmakeMove(m, moving, captured, prevC, prevE, prevH);

        if (!illegal) dests.push_back(m.to());
    }
    return dests;
}

bool tryMove(Position& pos, Square from, Square to) {
    MoveList list;
    generateMoves(pos, list);
    Colour us = pos.sideToMove;

    for (int i = 0; i < list.count; i++) {
        Move m = list.moves[i];
        if (m.from() != from || m.to() != to) continue;

        Piece moving = pos.pieceOn(m.from());
        uint8_t prevC = pos.castlingRights;
        Square prevE = pos.enPassantSquare;
        int prevH = pos.halfMoveClock;

        Piece captured = pos.makeMove(m);
        if (pos.isInCheck(us)) {
            pos.unmakeMove(m, moving, captured, prevC, prevE, prevH);
            continue;
        }
        return true;
    }
    return false;
}