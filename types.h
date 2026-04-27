#pragma once
#include <cstdint>
#include <string>

using Bitboard = uint64_t;

enum class Colour : uint8_t {
    White = 0,
    Black = 1,
    None = 2
};

enum class PieceType : uint8_t {
    Pawn = 0,
    Knight = 1,
    Bishop = 2,
    Rook = 3,
    Queen = 4,
    King = 5,
    None = 6
};

struct Piece {
    PieceType type = PieceType::None;
    Colour colour = Colour::None;

    bool isEmpty() const { return type == PieceType::None; }
};

using Square = uint8_t;
constexpr Square NO_SQUARE = 64;

constexpr Square makeSquare(int file, int rank) {
    return static_cast<Square>(rank * 8 + file);
}

constexpr int fileOf(Square sq) { return sq % 8; }
constexpr int rankOf(Square sq) { return sq / 8; }

// bits 0-5:   from square
// bits 6-11:  to square
// bits 12-13: promotion piece (0 = knight,1 = bishop,2 = rook,3 = queen)
// bits 14-15: move type (0 = normal,1 = promotion,2 = en passant,3 = castling)
struct Move {
    uint16_t data = 0;

    static Move make(Square from, Square to) {
        Move m;
        m.data = static_cast<uint16_t>(from | (to << 6));
        return m;
    }

    static Move makeCastle(Square from, Square to) {
        Move m;
        m.data = static_cast<uint16_t>(from | (to << 6) | (3 << 14));
        return m;
    }

    static Move makePromotion(Square from, Square to, PieceType promo) {
        uint16_t promoIndex = static_cast<uint16_t>(promo) - 1; // knight = 0
        Move m;
        m.data = static_cast<uint16_t>(
            from | (to << 6) | (promoIndex << 12) | (1 << 14));
        return m;
    }

    static Move makeEnPassant(Square from, Square to) {
        Move m;
        m.data = static_cast<uint16_t>(from | (to << 6) | (2 << 14));
        return m;
    }

    Square from()      const { return data & 0x3F; }
    Square to()        const { return (data >> 6) & 0x3F; }
    int    moveType()  const { return (data >> 14) & 0x3; }
    PieceType promo()  const {
        return static_cast<PieceType>(((data >> 12) & 0x3) + 1);
    }

    bool isPromotion()  const { return moveType() == 1; }
    bool isEnPassant()  const { return moveType() == 2; }
    bool isCastling()   const { return moveType() == 3; }
    bool isNull()       const { return data == 0; }

    bool operator==(const Move& other) const { return data == other.data; }
    bool operator!=(const Move& other) const { return data != other.data; }
};

constexpr Move NULL_MOVE = Move{};

constexpr Colour opponent(Colour c) {
    return c == Colour::White ? Colour::Black : Colour::White;
}