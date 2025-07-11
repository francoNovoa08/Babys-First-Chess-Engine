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