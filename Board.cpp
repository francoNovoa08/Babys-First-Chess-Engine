#include "Board.h"

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

bool Board::isMoveLegal(int fromRow, int fromColumn, int toRow, int toColumn, Colour turn) const {
	Piece piece = getPiece(fromRow, fromColumn);

	if (piece.colour != turn) {
		return false;
	}

	Piece destinationPiece = getPiece(toRow, toColumn);

	// Can't capture piece from same colour
	if (destinationPiece.colour == turn) {
		return false;
	}

	int destinationRow = toRow - fromRow;
	int destinationColumn = toColumn - fromColumn;

	// TODO: Add other piece rules
	switch (piece.type) {
	case PieceType::PAWN:
		if (piece.colour == Colour::WHITE) {
			// Move 1 square forward
			if (destinationColumn == 0 && destinationRow == 1 && destinationPiece.type == PieceType::NONE)
				return true;

			// Move 2 squares from start
			if (destinationColumn == 0 && destinationRow == 2 && fromRow == 1 &&
				destinationPiece.type == PieceType::NONE &&
				getPiece(fromRow + 1, fromColumn).type == PieceType::NONE)
				return true;

			// Capture
			if (std::abs(destinationColumn) == 1 && destinationRow == 1 &&
				destinationPiece.colour == Colour::BLACK)
				return true;
		}
		else {
			if (destinationColumn == 0 && destinationRow == -1 && destinationPiece.type == PieceType::NONE)
				return true;

			if (destinationColumn == 0 && destinationRow == -2 && fromRow == 6 &&
				destinationPiece.type == PieceType::NONE &&
				getPiece(fromRow - 1, fromColumn).type == PieceType::NONE)
				return true;

			if (std::abs(destinationColumn) == 1 && destinationRow == -1 &&
				destinationPiece.colour == Colour::WHITE)
				return true;
		}
		break;
	case PieceType::KNIGHT:
		if ((std::abs(destinationRow) == 2 && std::abs(destinationColumn) == 1) ||
			(std::abs(destinationRow) == 1 && std::abs(destinationColumn) == 2)) {
			return true;
		}
		break;
	case PieceType::BISHOP:
		if (std::abs(destinationRow) == std::abs(destinationColumn)) {
			int rowStep = (destinationRow > 0) ? 1 : -1;
			int colStep = (destinationColumn > 0) ? 1 : -1;
			for (int i = 1; i < std::abs(destinationRow); ++i) {
				if (getPiece(fromRow + i * rowStep, fromColumn + i * colStep).type != PieceType::NONE)
					return false;
			}
			return true;
		}
		break;
	case PieceType::ROOK:
		if (destinationRow == 0 || destinationColumn == 0) {
			int rowDir = (destinationRow == 0) ? 0 : (destinationRow > 0 ? 1 : -1);
			int colDir = (destinationColumn == 0) ? 0 : (destinationColumn > 0 ? 1 : -1);
			int steps = std::max(std::abs(destinationRow), std::abs(destinationColumn));
			for (int i = 1; i < steps; ++i) {
				if (getPiece(fromRow + i * rowDir, fromColumn + i * colDir).type != PieceType::NONE)
					return false;
			}
			return true;
		}
		break;
	case PieceType::QUEEN:
		// Try bishop movement
		if (std::abs(destinationRow) == std::abs(destinationColumn)) {
			int rowStep = (destinationRow > 0) ? 1 : -1;
			int colStep = (destinationColumn > 0) ? 1 : -1;
			for (int i = 1; i < std::abs(destinationRow); ++i) {
				if (getPiece(fromRow + i * rowStep, fromColumn + i * colStep).type != PieceType::NONE)
					return false;
			}
			return true;
		}
		// Try rook movement
		else if (destinationRow == 0 || destinationColumn == 0) {
			int rowDir = (destinationRow == 0) ? 0 : (destinationRow > 0 ? 1 : -1);
			int colDir = (destinationColumn == 0) ? 0 : (destinationColumn > 0 ? 1 : -1);
			int steps = std::max(std::abs(destinationRow), std::abs(destinationColumn));
			for (int i = 1; i < steps; ++i) {
				if (getPiece(fromRow + i * rowDir, fromColumn + i * colDir).type != PieceType::NONE)
					return false;
			}
			return true;
		}
		break;
	case PieceType::KING:
		if (std::abs(destinationRow) <= 1 && std::abs(destinationColumn) <= 1) {
			return true;
		}
		break;

	default:
		return false;
	}
	return false;
}