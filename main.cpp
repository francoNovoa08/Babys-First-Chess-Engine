#include <SFML/Graphics.hpp>
#include "Board.h"
#include "PieceSprites.h"
#include <iostream>
#include "Game.h"
#include <optional>

int main() {    
    Board board;

    Game game;
    
    const int windowSize = 864;
	const int squareSize = windowSize / 8;
    sf::RenderWindow window(
        sf::VideoMode(windowSize, windowSize),
        "Chess Engine"
    );

    PieceSprites spriteManager;
    if (!spriteManager.loadTextures()) {
        std::cerr << "Failed to load piece textures.\n";
        return -1; // Failed to load textures
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

			// Handle mouse click events
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
					int mouseX = event.mouseButton.x;
					int mouseY = event.mouseButton.y;

                    Square clicked = Game::getSquareFromMouse(mouseX, mouseY, squareSize);

                    if (game.getSelectedSquare().has_value()) {
                        // Try move
                        if (!game.tryMove(clicked.row, clicked.column, board)) {
							game.selectSquare(clicked.row, clicked.column);
						}
                    }
                    else {
						game.selectSquare(clicked.row, clicked.column);
                    }
                }
            }
        }

        window.clear();

		// Draw the chessboard
        for (int row = 0; row < 8; row++) {
            for (int column = 0; column < 8; column++) {
				// Draw squares
                sf::RectangleShape square(sf::Vector2f(squareSize, squareSize));
				square.setPosition(column * squareSize, row * squareSize);
				
				sf::Color lightSquare(240, 217, 181); // Cream
				sf::Color darkSquare(181, 136, 99); // Brown

                square.setFillColor((row + column) % 2 == 0 ? lightSquare : darkSquare);
				window.draw(square);

                // Highlight selected square
                if (game.getSelectedSquare().has_value()) {
					Square selected = game.getSelectedSquare().value();
                    if (selected.row == row && selected.column == column) {
						sf::RectangleShape highlight(sf::Vector2f(squareSize, squareSize));
						highlight.setPosition(column * squareSize, row * squareSize);
						highlight.setFillColor(sf::Color(255, 255, 0, 100));
						window.draw(highlight);
					}
                }

				// Draw pieces
				Piece piece = board.getPiece(row, column);
                if (piece.type != PieceType::NONE) {
                    sf::Sprite sprite = spriteManager.getPiece(piece);
					sprite.setPosition(column * squareSize, row * squareSize);
					window.draw(sprite);
                }
			}
        }
        
        window.display();

    }

    return 0;
}