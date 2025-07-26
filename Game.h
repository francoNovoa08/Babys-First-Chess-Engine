#ifndef GAME_H
#define GAME_H

#include "Board.h"
#include "Square.h"
#include <optional>

class Board;

class Game {
public:
	Game();

	void selectSquare(int row, int column);
	std::optional<Square> getSelectedSquare() const;
	void clearSelection();
	static Square getSquareFromMouse(int mouseX, int mouseY, int squareSize);
	bool tryMove(int targetRow, int targetColumn, Board& board);
	
private:
	std::optional<Square> selectedSquare;
	Colour currentTurn = Colour::WHITE;
};

#endif // GAME_H