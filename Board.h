#ifndef BOARD_H
#define BOARD_H

enum class PieceType { NONE, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING };
enum class Colour { NONE, WHITE, BLACK };

struct Piece {
	PieceType type = PieceType::NONE;
	Colour colour = Colour::NONE;
};

#endif // BOARD_H