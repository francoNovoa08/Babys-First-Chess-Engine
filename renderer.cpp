#include "Renderer.h"
#include <iostream>

bool Renderer::init() {
    window.create(
        sf::VideoMode(WINDOW_SIZE, WINDOW_SIZE),
        "Chess Engine",
        sf::Style::Titlebar | sf::Style::Close
    );
    window.setFramerateLimit(60);

    if (!sprites.loadTextures()) {
        std::cerr << "Failed to load piece textures\n";
        return false;
    }
    return true;
}

bool Renderer::pollEvents() {
    lastClick = std::nullopt;
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
            return false;
        }
        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left) {
            Square sq = squareFromMouse(event.mouseButton.x,
                event.mouseButton.y);
            if (sq != NO_SQUARE) lastClick = sq;
        }
    }
    return true;
}

Square Renderer::squareFromMouse(int x, int y) const {
    int file = x / SQUARE_SIZE;
    int rank = y / SQUARE_SIZE;
    if (file < 0 || file > 7 || rank < 0 || rank > 7) return NO_SQUARE;
    return makeSquare(file, 7 - rank);
}

void Renderer::drawBoard() {
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            sf::RectangleShape sq(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
            sq.setPosition(file * SQUARE_SIZE,
                (7 - rank) * SQUARE_SIZE);
            sq.setFillColor((file + rank) % 2 == 0
                ? darkSquare : lightSquare);
            window.draw(sq);
        }
    }
}

void Renderer::drawHighlights(std::optional<Square> selected,
    const std::vector<Square>& highlights) {
    auto drawOverlay = [&](Square sq, sf::Color color) {
        sf::RectangleShape overlay(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
        overlay.setPosition(fileOf(sq) * SQUARE_SIZE,
            (7 - rankOf(sq)) * SQUARE_SIZE);
        overlay.setFillColor(color);
        window.draw(overlay);
        };

    if (selected.has_value())
        drawOverlay(selected.value(), selectColor);

    for (Square sq : highlights)
        drawOverlay(sq, moveColor);
}

void Renderer::drawPieces(const Position& pos) {
    for (int sq = 0; sq < 64; sq++) {
        Piece piece = pos.pieceOn(static_cast<Square>(sq));
        if (piece.isEmpty()) continue;

        const sf::Sprite& sprite = sprites.getPiece(piece);

        float x = fileOf(sq) * SQUARE_SIZE;
        float y = (7 - rankOf(sq)) * SQUARE_SIZE;

        sf::Sprite s = sprite;
        s.setPosition(x, y);
        window.draw(s);
    }
}

void Renderer::render(const Position& pos,
    std::optional<Square> selected,
    const std::vector<Square>& highlights) {
    window.clear();
    drawBoard();
    drawHighlights(selected, highlights);
    drawPieces(pos);
    window.display();
}