#include <SFML/Graphics.hpp>
#include "Board.h"
#include "PieceSprites.h"
#include <iostream>

int main() {    
    Board board;
    
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
        }

        window.clear();

		// Draw the chessboard
        for (int row = 0; row < 8; row++) {
            for (int column = 0; column < 8; column++) {
				sf::RectangleShape square(sf::Vector2f(squareSize, squareSize));
				square.setPosition(column * squareSize, row * squareSize);
				
				sf::Color lightSquare(240, 217, 181); // Cream
				sf::Color darkSquare(181, 136, 99); // Brown

                square.setFillColor((row + column) % 2 == 0 ? lightSquare : darkSquare);
				window.draw(square);

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