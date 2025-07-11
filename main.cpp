#include <SFML/Graphics.hpp>
#include "board.h"

int main() {
    Board board;
    
    const int windowSize = 800;
	const int squareSize = windowSize / 8;
    sf::RenderWindow window(
        sf::VideoMode(windowSize, windowSize),
        "Chess Engine"
    );

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        window.clear();

        for (int row = 0; row < 8; row++) {
            for (int column = 0; column < 8; column++) {
				sf::RectangleShape square(sf::Vector2f(squareSize, squareSize));
				square.setPosition(column * squareSize, row * squareSize);
				square.setFillColor((row + column) % 2 == 0 ? sf::Color::White : sf::Color::Black);
				window.draw(square);

				Piece piece = board.getPiece(row, column);
                if (piece.type != PieceType::NONE) {
					sf::CircleShape pieceShape(squareSize / 3.0f);
					pieceShape.setPosition(column * squareSize + squareSize / 6.0f, row * squareSize + squareSize / 6.0f);
                    pieceShape.setFillColor(piece.colour == Colour::WHITE ? sf::Color::White : sf::Color::Black);
					window.draw(pieceShape);
                }
			}
        }
        
        window.display();

    }

    return 0;
}