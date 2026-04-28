#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include "types.h"

class PieceSprites {
public:
    bool loadTextures();
    const sf::Sprite& getPiece(Piece piece) const;

private:
    std::map<std::string, sf::Texture> textures;
    std::map<std::string, sf::Sprite>  sprites;

    std::string getKey(Piece piece) const;
};