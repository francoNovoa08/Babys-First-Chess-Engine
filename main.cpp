#include <SFML/Graphics.hpp>

int main() {
    const int windowSize = 800;
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
        
        window.display();

    }

    return 0;
}