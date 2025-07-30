#include "Board.h"
#include "Game.h"

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
	Piece oldPiece = board[row][column];

	if (oldPiece.type == PieceType::KING) {
		if (oldPiece.colour == Colour::WHITE) {
			whiteKingMoved = true;
		}
		else {
			blackKingMoved = true;
		}
	}
	if (oldPiece.type == PieceType::ROOK) {
		if (oldPiece.colour == Colour::WHITE) {
			if (row == 0 && column == 0) whiteQueenRookMoved = true;
			if (row == 0 && column == 7) whiteKingRookMoved = true;
		}
		else {
			if (row == 7 && column == 0) blackQueenRookMoved = true;
			if (row == 7 && column == 7) blackKingRookMoved = true;
		}
	}
	
	board[row][column] = piece;
}

void Board::promotePawn(int row, int column) {
	Piece& piece = board[row][column];
	if (piece.type == PieceType::PAWN) {
		if ((piece.colour == Colour::WHITE && row == 7) ||
			(piece.colour == Colour::BLACK && row == 1)) {
			piece.type = PieceType::QUEEN; 
		}
	}
}

// TODO: Add check logic in castling
bool Board::isCastlingMove(int fromRow, int fromColumn, int toRow, int toColumn, Colour turn) const {
	if (getPiece(fromRow, fromColumn).type != PieceType::KING) return false;
	if (turn == Colour::WHITE && whiteKingMoved) return false;
	if (turn == Colour::BLACK && blackKingMoved) return false;

	int row = (turn == Colour::WHITE) ? 0 : 7;

	// Kingside
	if (fromColumn == 4 && toColumn == 6 && fromRow == row && toRow == row) {
		if (turn == Colour::WHITE && whiteKingRookMoved) return false;
		if (turn == Colour::BLACK && blackKingRookMoved) return false;

		if (getPiece(row, 5).type != PieceType::NONE) return false;
		if (getPiece(row, 6).type != PieceType::NONE) return false;

		return true;
	}

	// Queenside
	if (fromColumn == 4 && toColumn == 2 && fromRow == row && toRow == row) {
		if (turn == Colour::WHITE && whiteQueenRookMoved) return false;
		if (turn == Colour::BLACK && blackQueenRookMoved) return false;

		if (getPiece(row, 1).type != PieceType::NONE) return false;
		if (getPiece(row, 2).type != PieceType::NONE) return false;
		if (getPiece(row, 3).type != PieceType::NONE) return false;

		return true;
	}

	return false;
}

bool Board::isMoveLegal(int fromRow, int fromColumn, int toRow, int toColumn, Colour turn) const {
	Piece piece = getPiece(fromRow, fromColumn);

	if (piece.colour != turn) {
		return false;
	}

	Piece destinationPiece = getPiece(toRow, toColumn);

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

		if (isCastlingMove(fromRow, fromColumn, toRow, toColumn, turn)) {
			return true;
		}

		break;

	default:
		return false;
	}
	return false;
}

Square Board::findKing(Colour colour) const {
	for (int row = 0; row < 8; row++) {
		for (int column = 0; column < 8; column++) {
			Piece piece = getPiece(row, column);
			if (piece.type == PieceType::KING && piece.colour == colour) {
				return Square { row, column };
			}
		}
	}

	return Square{ -1, -1 }; // King not found
}

bool Board::isSquareAttacked(int row, int column, Colour byColour) const {
	for (int attackerRow = 0; attackerRow < 8; attackerRow++) {
		for (int attackerColumn = 0; attackerColumn < 8; attackerColumn++) {
			Piece attacker = getPiece(attackerRow, attackerColumn);

			if (attacker.colour != byColour) continue;

			if (isMoveLegal(attackerRow, attackerColumn, row, column, byColour)) {
				return true; // Square is attacked
			}
		}
	}
	return false; // Square is not attacked
}
