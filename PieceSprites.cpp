#include "PieceSprites.h"
#include "renderer.h"
#include <iostream>

bool PieceSprites::loadTextures() {
    const std::string folder = "resources/pieces/";
    const std::string names[6] = {
        "pawn", "knight", "bishop", "rook", "queen", "king"
    };
    const Colour colours[2] = { Colour::White, Colour::Black };

    for (Colour colour : colours) {
        std::string prefix = (colour == Colour::White) ? "white_" : "black_";
        for (const std::string& name : names) {
            std::string key = prefix + name;
            std::string filePath = folder + key + ".png";

            sf::Texture texture;
            if (!texture.loadFromFile(filePath)) {
                std::cerr << "FAILED TO LOAD: " << filePath << "\n";
                return false;
            }

            textures[key] = texture;

            sf::Sprite sprite;
            sprite.setTexture(textures[key]);
            
            sf::Vector2u texSize = texture.getSize();
            float scaleX = static_cast<float>(Renderer::SQUARE_SIZE) / texSize.x;
            float scaleY = static_cast<float>(Renderer::SQUARE_SIZE) / texSize.y;
            sprite.setScale(scaleX, scaleY);

            sprites[key] = sprite;
        }
    }
    return true;
}

const sf::Sprite& PieceSprites::getPiece(Piece piece) const {
    static sf::Sprite emptySprite;
    if (piece.isEmpty()) return emptySprite;

    std::string key = getKey(piece);
    if (key.empty()) return emptySprite;

    return sprites.at(key);
}

std::string PieceSprites::getKey(Piece piece) const {
    if (piece.isEmpty()) return "";

    std::string colour = (piece.colour == Colour::White) ? "white_" : "black_";
    std::string name;

    switch (piece.type) {
    case PieceType::Pawn:   name = "pawn";   break;
    case PieceType::Knight: name = "knight";  break;
    case PieceType::Bishop: name = "bishop";  break;
    case PieceType::Rook:   name = "rook";    break;
    case PieceType::Queen:  name = "queen";   break;
    case PieceType::King:   name = "king";    break;
    default:                return "";
    }

    return colour + name;
}