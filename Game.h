#ifndef GAME_H
#define GAME_H

#include "Board.h"
#include <optional>

struct Square {
	int row;
	int column;
};

class Game {
public:
	Game();

	void selectSquare(int row, int column);
	std::optional<Square> getSelectedSquare() const;
	void clearSelection();
	static Square getSquareFromMouse(int mouseX, int mouseY, int squareSize);
	bool tryMove(int targetRow, int targetColumn, Board& Board);
	
private:
	std::optional<Square> selectedSquare;
};

#endif // GAME_H