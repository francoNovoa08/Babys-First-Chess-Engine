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
	bool whiteKingMoved = false;
	bool blackKingMoved = false;
	bool whiteKingRookMoved = false;
	bool whiteQueenRookMoved = false;
	bool blackKingRookMoved = false;
	bool blackQueenRookMoved = false;

	void initialise();

	Piece getPiece(int row, int column) const;
	void setPiece(int row, int column, Piece piece);
	bool isMoveLegal(int fromRow, int fromColumn, int toRow, int toColumn, Colour turn) const;
	void promotePawn(int row, int column);
	bool isCastlingMove(int fromRow, int fromColumn, int toRow, int toColumn, Colour turn) const;

private:
	std::array<std::array<Piece, 8>, 8> board;
};

#endif // BOARD_H