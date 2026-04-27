#include "position.h"
#include <cstring>

void Position::clear() {
    memset(pieces, 0, sizeof(pieces));
    memset(occupied, 0, sizeof(occupied));
    allOccupied = 0;
    sideToMove = Colour::White;
    castlingRights = ALL_CASTLING;
    enPassantSquare = NO_SQUARE;
    halfMoveClock = 0;
    fullMoveNumber = 1;
}

void Position::setStartingPosition() {
    clear();

    // White pieces
    putPiece({ PieceType::Rook,   Colour::White }, makeSquare(0, 0));
    putPiece({ PieceType::Knight, Colour::White }, makeSquare(1, 0));
    putPiece({ PieceType::Bishop, Colour::White }, makeSquare(2, 0));
    putPiece({ PieceType::Queen,  Colour::White }, makeSquare(3, 0));
    putPiece({ PieceType::King,   Colour::White }, makeSquare(4, 0));
    putPiece({ PieceType::Bishop, Colour::White }, makeSquare(5, 0));
    putPiece({ PieceType::Knight, Colour::White }, makeSquare(6, 0));
    putPiece({ PieceType::Rook,   Colour::White }, makeSquare(7, 0));
    for (int f = 0; f < 8; f++)
        putPiece({ PieceType::Pawn, Colour::White }, makeSquare(f, 1));

    // Black pieces
    putPiece({ PieceType::Rook,   Colour::Black }, makeSquare(0, 7));
    putPiece({ PieceType::Knight, Colour::Black }, makeSquare(1, 7));
    putPiece({ PieceType::Bishop, Colour::Black }, makeSquare(2, 7));
    putPiece({ PieceType::Queen,  Colour::Black }, makeSquare(3, 7));
    putPiece({ PieceType::King,   Colour::Black }, makeSquare(4, 7));
    putPiece({ PieceType::Bishop, Colour::Black }, makeSquare(5, 7));
    putPiece({ PieceType::Knight, Colour::Black }, makeSquare(6, 7));
    putPiece({ PieceType::Rook,   Colour::Black }, makeSquare(7, 7));
    for (int f = 0; f < 8; f++)
        putPiece({ PieceType::Pawn, Colour::Black }, makeSquare(f, 6));

    updateOccupancy();
}

void Position::putPiece(Piece piece, Square sq) {
    int c = static_cast<int>(piece.colour);
    int pt = static_cast<int>(piece.type);
    pieces[c][pt] |= squareBB(sq);
}

void Position::removePiece(Square sq) {
    Bitboard mask = ~squareBB(sq);
    for (int c = 0; c < 2; c++)
        for (int pt = 0; pt < 6; pt++)
            pieces[c][pt] &= mask;
}

void Position::updateOccupancy() {
    for (int c = 0; c < 2; c++) {
        occupied[c] = 0;
        for (int pt = 0; pt < 6; pt++)
            occupied[c] |= pieces[c][pt];
    }
    allOccupied = occupied[0] | occupied[1];
}

Piece Position::pieceOn(Square sq) const {
    Bitboard bb = squareBB(sq);
    for (int c = 0; c < 2; c++) {
        for (int pt = 0; pt < 6; pt++) {
            if (pieces[c][pt] & bb)
                return { static_cast<PieceType>(pt),
                        static_cast<Colour>(c) };
        }
    }
    return { PieceType::None, Colour::None };
}

PieceType Position::typeOn(Square sq) const {
    return pieceOn(sq).type;
}

Colour Position::colourOn(Square sq) const {
    return pieceOn(sq).colour;
}

Piece Position::makeMove(Move move) {
    Square from = move.from();
    Square to = move.to();

    Piece moving = pieceOn(from);
    Piece captured = pieceOn(to);

    int c = static_cast<int>(moving.colour);
    int pt = static_cast<int>(moving.type);

    // Remove moving piece from source
    pieces[c][pt] &= ~squareBB(from);

    // Remove captured piece
    if (!captured.isEmpty()) {
        int cc = static_cast<int>(captured.colour);
        int cpt = static_cast<int>(captured.type);
        pieces[cc][cpt] &= ~squareBB(to);
    }

    if (move.isCastling()) {
        Square rookFrom, rookTo;
        if (to > from) { // Kingside
            rookFrom = makeSquare(7, rankOf(from));
            rookTo = makeSquare(5, rankOf(from));
        }
        else {         // Queenside
            rookFrom = makeSquare(0, rankOf(from));
            rookTo = makeSquare(3, rankOf(from));
        }
        int rpt = static_cast<int>(PieceType::Rook);
        pieces[c][rpt] &= ~squareBB(rookFrom);
        pieces[c][rpt] |= squareBB(rookTo);
    }

    if (move.isEnPassant()) {
        Square epPawn = makeSquare(fileOf(to), rankOf(from));
        int opp = 1 - c;
        int ppt = static_cast<int>(PieceType::Pawn);
        pieces[opp][ppt] &= ~squareBB(epPawn);
        captured = { PieceType::Pawn,
                    static_cast<Colour>(opp) };
    }

    if (move.isPromotion()) {
        int promoType = static_cast<int>(move.promo());
        pieces[c][promoType] |= squareBB(to);
    }
    else {
        pieces[c][pt] |= squareBB(to);
    }

    enPassantSquare = NO_SQUARE;
    if (moving.type == PieceType::Pawn) {
        int rankDiff = rankOf(to) - rankOf(from);
        if (rankDiff == 2 || rankDiff == -2) {
            enPassantSquare = makeSquare(fileOf(from),
                (rankOf(from) + rankOf(to)) / 2);
        }
    }

    auto revokeCastling = [&](Square sq) {
        switch (sq) {
        case 0:  castlingRights &= ~WHITE_OOO; break;
        case 4:  castlingRights &= ~(WHITE_OO | WHITE_OOO); break;
        case 7:  castlingRights &= ~WHITE_OO;  break;
        case 56: castlingRights &= ~BLACK_OOO; break;
        case 60: castlingRights &= ~(BLACK_OO | BLACK_OOO); break;
        case 63: castlingRights &= ~BLACK_OO;  break;
        }
        };
    revokeCastling(from);
    revokeCastling(to);

    if (moving.type == PieceType::Pawn || !captured.isEmpty())
        halfMoveClock = 0;
    else
        halfMoveClock++;

    if (sideToMove == Colour::Black) fullMoveNumber++;
    sideToMove = opponent(sideToMove);

    updateOccupancy();
    return captured;
}

void Position::unmakeMove(Move move, Piece moving, Piece captured,
    uint8_t prevCastling, Square prevEP,
    int prevHalfClock) {
    sideToMove = opponent(sideToMove);
    if (sideToMove == Colour::Black) fullMoveNumber--;

    Square from = move.from();
    Square to = move.to();
    int c = static_cast<int>(sideToMove);

    if (move.isPromotion()) {
        int promoType = static_cast<int>(move.promo());
        pieces[c][promoType] &= ~squareBB(to);
    }
    else {
        int pt = static_cast<int>(moving.type);
        pieces[c][pt] &= ~squareBB(to);
    }

    pieces[c][static_cast<int>(moving.type)] |= squareBB(from);

    if (!captured.isEmpty() && !move.isEnPassant()) {
        int cc = static_cast<int>(captured.colour);
        int cpt = static_cast<int>(captured.type);
        pieces[cc][cpt] |= squareBB(to);
    }

    if (move.isEnPassant()) {
        Square epPawn = makeSquare(fileOf(to), rankOf(from));
        int opp = 1 - c;
        pieces[opp][static_cast<int>(PieceType::Pawn)] |= squareBB(epPawn);
    }

    if (move.isCastling()) {
        Square rookFrom, rookTo;
        if (to > from) {
            rookFrom = makeSquare(7, rankOf(from));
            rookTo = makeSquare(5, rankOf(from));
        }
        else {
            rookFrom = makeSquare(0, rankOf(from));
            rookTo = makeSquare(3, rankOf(from));
        }
        int rpt = static_cast<int>(PieceType::Rook);
        pieces[c][rpt] |= squareBB(rookFrom);
        pieces[c][rpt] &= ~squareBB(rookTo);
    }

    castlingRights = prevCastling;
    enPassantSquare = prevEP;
    halfMoveClock = prevHalfClock;

    updateOccupancy();
}

bool Position::isSquareAttacked(Square sq, Colour byColour) const {
    int c = static_cast<int>(byColour);

    Bitboard sqBB = squareBB(sq);
    Bitboard pawns = getPieces(byColour, PieceType::Pawn);
    Bitboard pawnAttacks = (byColour == Colour::White)
        ? (shiftNE(pawns) | shiftNW(pawns))
        : (shiftSE(pawns) | shiftSW(pawns));
    if (pawnAttacks & sqBB) return true;

    Bitboard knights = getPieces(byColour, PieceType::Knight);
    Bitboard bb = knights;
    while (bb) {
        Square kSq = popLsb(bb);
        int f = fileOf(kSq), r = rankOf(kSq);
        const int df[] = { 1,2,2,1,-1,-2,-2,-1 };
        const int dr[] = { 2,1,-1,-2,-2,-1,1,2 };
        for (int i = 0; i < 8; i++) {
            int nf = f + df[i], nr = r + dr[i];
            if (nf >= 0 && nf < 8 && nr >= 0 && nr < 8)
                if (makeSquare(nf, nr) == sq) return true;
        }
    }

    Bitboard diagAttackers = getPieces(byColour, PieceType::Bishop)
        | getPieces(byColour, PieceType::Queen);
    if (bishopAttacks(sq, allOccupied) & diagAttackers) return true;

    Bitboard straightAttackers = getPieces(byColour, PieceType::Rook)
        | getPieces(byColour, PieceType::Queen);
    if (rookAttacks(sq, allOccupied) & straightAttackers) return true;

    Bitboard king = getPieces(byColour, PieceType::King);
    if (king) {
        Square kSq = lsb(king);
        int f = fileOf(kSq), r = rankOf(kSq);
        int tf = fileOf(sq), tr = rankOf(sq);
        if (std::abs(f - tf) <= 1 && std::abs(r - tr) <= 1) return true;
    }

    return false;
}