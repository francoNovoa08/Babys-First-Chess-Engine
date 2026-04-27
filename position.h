#pragma once
#include "types.h"
#include "bitboard.h"
#include <array>
#include <optional>
#include <cassert>

// Castling
// bit 0: white kingside
// bit 1: white queenside
// bit 2: black kingside
// bit 3: black queenside

enum CastlingRights : uint8_t {
    NO_CASTLING = 0,
    WHITE_OO = 1,
    WHITE_OOO = 2,
    BLACK_OO = 4,
    BLACK_OOO = 8,
    ALL_CASTLING = 15
};

struct Position {
    // pieces[colour][pieceType]
    Bitboard pieces[2][6] = {};

    Bitboard occupied[2] = {};  // occupied[0] = white, occupied[1] = black
    Bitboard allOccupied = 0;

    Colour sideToMove = Colour::White;

    uint8_t castlingRights = ALL_CASTLING;

    Square enPassantSquare = NO_SQUARE;

    // For 50-move rule
    int halfMoveClock = 0;

    int fullMoveNumber = 1;

    void setStartingPosition();
    void clear();


    void putPiece(Piece piece, Square sq);

    void removePiece(Square sq);

    Piece pieceOn(Square sq) const;

    PieceType typeOn(Square sq) const;

    Colour colourOn(Square sq) const;

    bool isEmpty(Square sq) const {
        return !testBit(allOccupied, sq);
    }

    void updateOccupancy();

    Bitboard getPieces(Colour c, PieceType pt) const {
        return pieces[static_cast<int>(c)][static_cast<int>(pt)];
    }

    Bitboard getOccupied(Colour c) const {
        return occupied[static_cast<int>(c)];
    }

    Bitboard kingSquareBB(Colour c) const {
        return getPieces(c, PieceType::King);
    }

    Square kingSquare(Colour c) const {
        return lsb(kingSquareBB(c));
    }


    Piece makeMove(Move move);

    void unmakeMove(Move move, Piece moving, Piece captured,
        uint8_t prevCastling, Square prevEP,
        int prevHalfClock);

    bool isSquareAttacked(Square sq, Colour byColour) const;
    bool isInCheck(Colour c) const {
        return isSquareAttacked(kingSquare(c), opponent(c));
    }
};