#pragma once
#include "types.h"
#include <bit>
#include <cassert>

constexpr Bitboard squareBB(Square sq) {
    return Bitboard(1) << sq;
}

constexpr bool testBit(Bitboard bb, Square sq) {
    return (bb >> sq) & 1;
}

constexpr Bitboard setBit(Bitboard bb, Square sq) {
    return bb | squareBB(sq);
}

constexpr Bitboard clearBit(Bitboard bb, Square sq) {
    return bb & ~squareBB(sq);
}

inline int popcount(Bitboard bb) {
    return std::popcount(bb);
}

inline Square lsb(Bitboard bb) {
    assert(bb != 0);
    return static_cast<Square>(std::countr_zero(bb));
}

inline Square popLsb(Bitboard& bb) {
    Square sq = lsb(bb);
    bb &= bb - 1;
    return sq;
}

constexpr Bitboard FILE_A = 0x0101010101010101ULL;
constexpr Bitboard FILE_B = FILE_A << 1;
constexpr Bitboard FILE_G = FILE_A << 6;
constexpr Bitboard FILE_H = FILE_A << 7;
constexpr Bitboard RANK_1 = 0xFFULL;
constexpr Bitboard RANK_2 = RANK_1 << 8;
constexpr Bitboard RANK_3 = RANK_1 << 16;
constexpr Bitboard RANK_4 = RANK_1 << 24;
constexpr Bitboard RANK_5 = RANK_1 << 32;
constexpr Bitboard RANK_6 = RANK_1 << 40;
constexpr Bitboard RANK_7 = RANK_1 << 48;
constexpr Bitboard RANK_8 = RANK_1 << 56;

constexpr Bitboard shiftN(Bitboard b) { return b << 8; }
constexpr Bitboard shiftS(Bitboard b) { return b >> 8; }
constexpr Bitboard shiftE(Bitboard b) { return (b & ~FILE_H) << 1; }
constexpr Bitboard shiftW(Bitboard b) { return (b & ~FILE_A) >> 1; }
constexpr Bitboard shiftNE(Bitboard b) { return (b & ~FILE_H) << 9; }
constexpr Bitboard shiftNW(Bitboard b) { return (b & ~FILE_A) << 7; }
constexpr Bitboard shiftSE(Bitboard b) { return (b & ~FILE_H) >> 7; }
constexpr Bitboard shiftSW(Bitboard b) { return (b & ~FILE_A) >> 9; }

inline Bitboard rayAttacks(Square sq, Bitboard occupied,
    int fileStep, int rankStep) {
    Bitboard attacks = 0;
    int file = fileOf(sq);
    int rank = rankOf(sq);

    file += fileStep;
    rank += rankStep;

    while (file >= 0 && file < 8 && rank >= 0 && rank < 8) {
        Square s = makeSquare(file, rank);
        attacks |= squareBB(s);
        if (testBit(occupied, s)) break; 
        file += fileStep;
        rank += rankStep;
    }
    return attacks;
}

inline Bitboard rookAttacks(Square sq, Bitboard occupied) {
    return rayAttacks(sq, occupied, 1, 0)
        | rayAttacks(sq, occupied, -1, 0)
        | rayAttacks(sq, occupied, 0, 1)
        | rayAttacks(sq, occupied, 0, -1);
}

inline Bitboard bishopAttacks(Square sq, Bitboard occupied) {
    return rayAttacks(sq, occupied, 1, 1)
        | rayAttacks(sq, occupied, -1, 1)
        | rayAttacks(sq, occupied, 1, -1)
        | rayAttacks(sq, occupied, -1, -1);
}

inline Bitboard queenAttacks(Square sq, Bitboard occupied) {
    return rookAttacks(sq, occupied) | bishopAttacks(sq, occupied);
}