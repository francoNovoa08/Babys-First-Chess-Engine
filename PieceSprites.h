/*
#ifndef PIECESPRITES_H
#define PIECESPRITES_H

#include <SFML/Graphics.hpp>
#include <map>

class PieceSprites {
public:
	bool loadTextures();
	const sf::Sprite& getPiece(Piece piece) const;

private:
	std::map<std::string, sf::Texture> textures;
	std::map<std::string, sf::Sprite> sprites;

	std::string getKey(Piece piece) const;
};


#endif // PIECESPRITES_H
*/
