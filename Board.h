#ifndef BOARD_H
#define BOARD_H

enum class PieceType { NONE, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING };
enum class Colour { NONE, WHITE, BLACK };

struct Piece {
	PieceType type = PieceType::NONE;
	Colour colour = Colour::NONE;
};

#include <array>

class Board {
public:
	Board();

	void initialise();

	Piece getPiece(int row, int column) const;
	void setPiece(int row, int column, Piece piece);

private:
	std::array<std::array<Piece, 8>, 8> board;
};

#endif // BOARD_H