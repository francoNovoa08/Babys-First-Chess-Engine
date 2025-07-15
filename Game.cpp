#include "Game.h"

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
	
	// Check castling
	if (piece.type == PieceType::KING && std::abs(targetColumn - from.column) == 2) {
		int row = from.row;

		if (targetColumn == 6) { // Kingside
			Piece rook = board.getPiece(row, 7);
			board.setPiece(row, 5, rook);
			board.setPiece(row, 7, Piece{});
		}
		else if (targetColumn == 2) { // Queenside
			Piece rook = board.getPiece(row, 0);
			board.setPiece(row, 3, rook);
			board.setPiece(row, 0, Piece{});
		}
	}

	board.setPiece(targetRow, targetColumn, piece);
	board.setPiece(from.row, from.column, Piece{});
	board.promotePawn(targetRow, targetColumn);

	currentTurn = (currentTurn == Colour::WHITE) ? Colour::BLACK : Colour::WHITE;

	clearSelection();
	return true;
}