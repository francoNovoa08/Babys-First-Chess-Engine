#pragma once
#include "position.h"
#include "movegen.h"

std::vector<Square> legalDestinations(Position& pos, Square from);

bool tryMove(Position& pos, Square from, Square to);