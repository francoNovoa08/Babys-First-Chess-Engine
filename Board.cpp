#include "board.h"

Board::Board() {
	initialise();
}

/*
	WHITE
	0 [0, 1, 2, 3, 4, 5, 6, 7],
	1 [0, 1, 2, 3, 4, 5, 6, 7],
	2 [0, 1, 2, 3, 4, 5, 6, 7],
	3 [0, 1, 2, 3, 4, 5, 6, 7],
	4 [0, 1, 2, 3, 4, 5, 6, 7],
	5 [0, 1, 2, 3, 4, 5, 6, 7],
	6 [0, 1, 2, 3, 4, 5, 6, 7],
	7 [0, 1, 2, 3, 4, 5, 6, 7]
	BLACK
*/

void Board::initialise() {
	// Clear board
	for (int row = 0; row < 8; row++) {
		for (int columns = 0; columns < 8; columns++) {
			board[row][columns] = { PieceType::NONE, Colour::NONE };
		}
	}

	// Set pawns
	for (int column = 0; column < 8; column++) {
		board[1][column] = { PieceType::PAWN, Colour::WHITE };
		board[6][column] = { PieceType::PAWN, Colour::BLACK };
	}

	// Set Knights
	board[0][1] = { PieceType::KNIGHT, Colour::WHITE };
	board[0][6] = { PieceType::KNIGHT, Colour::WHITE };

	board[7][1] = { PieceType::KNIGHT, Colour::BLACK };
	board[7][6] = { PieceType::KNIGHT, Colour::BLACK };

	// Set Bishops
	board[0][2] = { PieceType::BISHOP, Colour::WHITE };
	board[0][5] = { PieceType::BISHOP, Colour::WHITE };

	board[7][2] = { PieceType::BISHOP, Colour::BLACK };
	board[7][5] = { PieceType::BISHOP, Colour::BLACK };

	// Set Rooks
	board[0][0] = { PieceType::ROOK, Colour::WHITE };
	board[0][7] = { PieceType::ROOK, Colour::WHITE };

	board[7][0] = { PieceType::ROOK, Colour::BLACK };
	board[7][7] = { PieceType::ROOK, Colour::BLACK };

	// Set Queens
	board[0][3] = { PieceType::QUEEN, Colour::WHITE };
	board[7][3] = { PieceType::QUEEN, Colour::BLACK };

	// Set Kings
	board[0][4] = { PieceType::KING, Colour::WHITE };
	board[7][4] = { PieceType::KING, Colour::BLACK };
}

Piece Board::getPiece(int row, int column) const {
	return board[row][column];
}

void Board::setPiece(int row, int column, Piece piece) {
	board[row][column] = piece;
}