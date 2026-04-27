#pragma once
#include "position.h"
#include <vector>

struct MoveList {
    Move moves[256];
    int  count = 0;

    void add(Move m) { moves[count++] = m; }
    Move* begin() { return moves; }
    Move* end() { return moves + count; }
};

// (Not including moves that cause a check)
void generateMoves(const Position& pos, MoveList& list);

void generateCaptures(const Position& pos, MoveList& list);

bool hasLegalMove(Position& pos);