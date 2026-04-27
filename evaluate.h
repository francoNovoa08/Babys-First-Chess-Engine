#pragma once
#include "position.h"

/**
* @brief Gets the score from the perspective of the side to move
* Positive = good for side to move, negative = bad
* 100 = 1 pawn of advantage
* @param pos The board position
* @return The score
**/
int evaluate(const Position& pos);

constexpr int PAWN_VALUE = 100;
constexpr int KNIGHT_VALUE = 320;
constexpr int BISHOP_VALUE = 330;
constexpr int ROOK_VALUE = 500;
constexpr int QUEEN_VALUE = 900;
constexpr int KING_VALUE = 20000;

constexpr int MATE_SCORE = 100000;
constexpr int INF_SCORE = 200000;

