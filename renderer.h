#pragma once
#include <SFML/Graphics.hpp>
#include "position.h"
#include "PieceSprites.h"
#include <optional>

struct SquareCoord {
    int file;
    int rank;
};

class Renderer {
public:
    static constexpr int WINDOW_SIZE = 864;
    static constexpr int SQUARE_SIZE = WINDOW_SIZE / 8;

    bool init();
    bool isOpen() const { return window.isOpen(); }

    bool pollEvents();

    void render(const Position& pos,
        std::optional<Square> selected,
        const std::vector<Square>& highlights);

    Square squareFromMouse(int x, int y) const;

    std::optional<Square> lastClick;

private:
    sf::RenderWindow window;
    PieceSprites     sprites;

    sf::Color lightSquare{ 240, 217, 181 };
    sf::Color darkSquare{ 181, 136,  99 };
    sf::Color selectColor{ 255, 255,   0, 120 };
    sf::Color moveColor{ 0, 255,   0,  80 };

    void drawBoard();
    void drawHighlights(std::optional<Square> selected,
        const std::vector<Square>& highlights);
    void drawPieces(const Position& pos);
};