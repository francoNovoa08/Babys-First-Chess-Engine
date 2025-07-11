#include "PieceSprites.h"
#include <iostream>

bool PieceSprites::loadTextures() {
	const std::string folder = "resources/pieces/";
	const std::string names[6] = {
		"pawn",
		"knight",
		"bishop",
		"rook",
		"queen",
		"king"
	};

	const Colour colours[2] = {
		Colour::WHITE,
		Colour::BLACK
	};

	for (Colour colour : colours) {
		std::string prefix = (colour == Colour::WHITE) ? "white_" : "black_";
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
			sprite.setScale(0.82f, 0.82f); // Scaled for 108x108 squares
			sprites[key] = sprite;
		}
	}

	return true; // All textures loaded successfully
}

// Get piece sprite based on piece type and colour
const sf::Sprite& PieceSprites::getPiece(Piece piece) const {
	static sf::Sprite emptySprite;

	if (piece.type == PieceType::NONE) {
		return emptySprite; 
	}

	std::string colour = (piece.colour == Colour::WHITE) ? "white_" : "black_";
	std::string name;

	switch (piece.type) {
	case PieceType::PAWN: 			
		name = "pawn";
		break;
	case PieceType::KNIGHT:			
		name = "knight";
		break;
	case PieceType::BISHOP:			
		name = "bishop";
		break;
	case PieceType::ROOK:			
		name = "rook";
		break;
	case PieceType::QUEEN:			
		name = "queen";
		break;
	case PieceType::KING:			
		name = "king";
		break;
	default:
		return emptySprite; // Invalid piece type
	}

	std::string key = colour + name;
	return sprites.at(key);
}

std::string PieceSprites::getKey(Piece piece) const {
	if (piece.type == PieceType::NONE) {
		return "";
	}

	std::string colour = (piece.colour == Colour::WHITE) ? "white_" : "black_";
	std::string name;

	switch (piece.type) {
		case PieceType::PAWN:
			name = "pawn";
			break;
		case PieceType::KNIGHT:
			name = "knight";
			break;
		case PieceType::BISHOP:
			name = "bishop";
			break;
		case PieceType::ROOK:
			name = "rook";
			break;
		case PieceType::QUEEN:
			name = "queen";
			break;
		case PieceType::KING:
			name = "king";
			break;
		default:
			return ""; // Invalid piece type
	}
	return colour + name;
}