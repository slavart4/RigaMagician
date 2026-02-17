#include "Bitboards.h"

namespace Rmagician {

Bitboards::Bitboards() {
    piece_bb_.fill(0ULL);
    color_bb_.fill(0ULL);
    occupied_ = 0ULL;
    board_.fill(PIECE_NUM);
}

void Bitboards::print() {
    for (auto pc : board_) {
        std::cout << pieces_notations[pc] << std::endl;
    }
}

Bitboard Bitboards::pieces(Color c) const {
    return color_bb_[c];
}

Bitboard Bitboards::pieces(PieceType pt, Color c) const {
    if (c == WHITE) {
        return piece_bb_[pt];
    }
    return piece_bb_[pt + 6];
}

Bitboard Bitboards::all() const {
    return occupied_;
}

/*!
 * @details
 * 1. Bit OR operation, left all pieces on their places and
 * set new one to the square. Example on small board:
 *  1 1 1    0 0 0    1 1 1
 *  0 0 0 |= 1 0 0 -> 1 0 0
 *  1 1 1    0 0 0    1 1 1
 *
 * 2. Color c = (piece < 6) ? WHITE : BLACK; bcs in Piece enum
 * black pieces starts from 6.
 *
 * @note Shift bit to get square mask is better than pre-calculate it
 * because it's faster than memory-access.
 */
void Bitboards::set_piece(Square square, Piece piece) {
    Bitboard mask = 1ULL << square;
    Color c = (piece < 6) ? WHITE : BLACK;

    board_[square] = piece;
    piece_bb_[piece] |= mask;
    color_bb_[c] |= mask;
    occupied_ |= mask;
}

/**
 * @details
 * Bit AND(&) operation and NO(~).
 *        1 1 1
 * Board  1 0 0
 *        1 1 1
 *
 *        0 0 0    1 1 1
 * Mask ~ 1 0 0 -> 0 1 1
 *        0 0 0    1 1 1
 *
 *        1 1 1    1 1 1    1 1 1
 * AND    1 0 0 &= 0 1 1 -> 0 0 0
 *        1 1 1    1 1 1    1 1 1
 *
 * @note Do not use XOR because we can add piece instead of
 * removing it if there was no piece on this square.
 */
void Bitboards::remove_piece(Square square) {
    Piece piece = board_[square];
    if (piece == PIECE_NUM) {
        return;
    }

    Bitboard mask = 1ULL << square;
    Color c = (piece < 6) ? WHITE : BLACK;

    piece_bb_[piece] &= ~mask;
    color_bb_[c] &= ~mask;
    occupied_ &= ~mask;
    board_[square] = PIECE_NUM;
}

/*!
 * @details
 * 1. Remove piece from destination square (in case if it capture).
 * 2. Remove piece from source square.
 * 3. Set piece from source square to destination square.
 */
void Bitboards::move_piece(Square from, Square to) {
    Piece piece = board_[from];

    remove_piece(to); // if there was a piece - capture
    remove_piece(from);
    set_piece(to, piece);
}

}
