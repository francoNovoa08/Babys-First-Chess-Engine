#pragma once
#include "position.h"
#include "search.h"
#include <string>

// Convert Square to UCI notation
std::string squareToStr(Square sq);

// Convert UCI string to Square
Square strToSquare(const std::string& s);


std::string moveToStr(Move m);
Move strToMove(const std::string& s, const Position& pos);

void uciLoop();
