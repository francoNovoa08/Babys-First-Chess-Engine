#include "Game.h"
#include "Board.h"
#include <iostream>

Game::Game() : selectedSquare(std::nullopt) {}

void Game::selectSquare(int row, int column) {
	selectedSquare = Square{ row, column };
}

std::optional<Square> Game::getSelectedSquare() const {
	return selectedSquare;
}

void Game::clearSelection() {
	selectedSquare = std::nullopt;
}

Square Game::getSquareFromMouse(int mouseX, int mouseY, int squareSize) {
	return Square{
		mouseY / squareSize,
		mouseX / squareSize
	};
}

bool Game::tryMove(int targetRow, int targetColumn, Board& board) {
	if (!selectedSquare.has_value()) {
		return false; // No square selected
	}

	Square from = selectedSquare.value();
	Piece piece = board.getPiece(from.row, from.column);

	if (piece.type == PieceType::NONE || piece.colour != currentTurn) {
		return false;
	}

	if (!board.isMoveLegal(from.row, from.column, targetRow, targetColumn, currentTurn)) {
		return false;
	}

	std::cout << "Selected: " << from.row << "," << from.column
		<< " → Target: " << targetRow << "," << targetColumn << "\n";

	// Handle castling rook movement BEFORE we check for check conditions
	if (piece.type == PieceType::KING && board.isCastlingMove(from.row, from.column, targetRow, targetColumn, currentTurn)) {
		// Move the rook manually based on side
		int row = from.row;
		if (targetColumn == 6) {
			// Kingside castling
			board.setPiece(row, 5, { PieceType::ROOK, currentTurn }); // f-file
			board.setPiece(row, 7, { PieceType::NONE, Colour::NONE }); // clear h-file
		}
		else if (targetColumn == 2) {
			// Queenside castling
			board.setPiece(row, 3, { PieceType::ROOK, currentTurn }); // d-file
			board.setPiece(row, 0, { PieceType::NONE, Colour::NONE }); // clear a-file
		}
	}

	Piece captured = board.getPiece(targetRow, targetColumn);

	board.setPiece(targetRow, targetColumn, piece);
	board.setPiece(from.row, from.column, Piece{});

	std::cout << "Moved piece: " << (int)piece.type
		<< " from (" << from.row << "," << from.column << ")"
		<< " to (" << targetRow << "," << targetColumn << ")\n";

	Square kingSquare;
	if (piece.type == PieceType::KING) {
		kingSquare = Square{ targetRow, targetColumn };
	}
	else {
		kingSquare = board.findKing(currentTurn);
	}

	if (board.isSquareAttacked(kingSquare.row, kingSquare.column,
		currentTurn == Colour::WHITE ? Colour::BLACK : Colour::WHITE)) {
		// Undo move
		board.setPiece(from.row, from.column, piece);
		board.setPiece(targetRow, targetColumn, captured);

		// Undo rook move if castling
		if (piece.type == PieceType::KING && board.isCastlingMove(from.row, from.column, targetRow, targetColumn, currentTurn)) {
			int row = from.row;
			if (targetColumn == 6) {
				board.setPiece(row, 7, { PieceType::ROOK, currentTurn }); // h-file
				board.setPiece(row, 5, { PieceType::NONE, Colour::NONE }); // f-file
			}
			else if (targetColumn == 2) {
				board.setPiece(row, 0, { PieceType::ROOK, currentTurn }); // a-file
				board.setPiece(row, 3, { PieceType::NONE, Colour::NONE }); // d-file
			}
		}
		return false;
	}

	board.promotePawn(targetRow, targetColumn);
	currentTurn = (currentTurn == Colour::WHITE) ? Colour::BLACK : Colour::WHITE;

	clearSelection();
	return true;
}
