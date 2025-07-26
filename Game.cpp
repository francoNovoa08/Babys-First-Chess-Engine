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
		board.setPiece(from.row, from.column, piece);
		board.setPiece(targetRow, targetColumn, captured);
		return false;
	}

	board.promotePawn(targetRow, targetColumn);
	currentTurn = (currentTurn == Colour::WHITE) ? Colour::BLACK : Colour::WHITE;

	clearSelection();
	return true;
}