#ifndef RIGAMAGICIAN_BITBOARD_H
#define RIGAMAGICIAN_BITBOARD_H

#include <array>
#include "Types.h"

///TODO: Add to brief info about what optimization gives every bitboard member.
/// Because its not obvious why we need them, when we have board member.

namespace Rmagician {

class Bitboards {
public:
    Bitboards();

    /*!
     * Prints board for debug.
     */
    void print();

    /*!
     * @brief Returns all squares occupied white/black pieces.
     * @param c Color of pieces we want to get.
     * @return Bitboard (uint64_t)
     */
    Bitboard pieces(Color c) const;

    /*!
     * @brief Returns squares occupied by all white/black pieces of some type.
     * @param pt PieceType of pieces we want to get.
     * @param c  Color of pieces we want to get.
     * @return Bitboard (uint64_t)
     */
    Bitboard pieces(PieceType pt, Color c) const;

    /*!
     * @brief Returns all occupied squares.
     * @return Bitboard (uint64_t)
     */
    Bitboard all() const;

    /*!
     * @brief Set piece on some square.
     * @param square Square to place piece
     * @param piece Piece to place
     */
    void set_piece(Square square, Piece piece);

    /*!
     * @brief Remove piece from some square.
     * @param square Square to remove piece on it
     */
    void remove_piece(Square square);

    /*!
     * @brief Move piece from one square to another.
     * @param from Square to get piece from
     * @param to Square to set piece on it
     */
    void move_piece(Square from, Square to);

    /*!
     * @brief Returns piece from specified square.
     * @param sq Square to get piece from it
     * @return Piece
     */
    Piece piece_on(Square sq) const { return board_[sq]; }

private:
    /*!
     * Bitboard for each piece type (W_PAWN, B_KNIGHT, etc...).
     * Shows all squares occupied by those pieces.
     */
    std::array<Bitboard, PIECE_NUM> piece_bb_{};

    /*!
     * Bitboard for each color. Shows all squares occupied by
     * all pieces of some color.
     */
    std::array<Bitboard, COLOR_NUM> color_bb_{};

    /*!
     * Bitboard for all pieces. Shows all occupied squares.
     */
    Bitboard occupied_ = 0;

    /*!
     * Array represents all pieces on boards.
     */
    std::array<Piece, 64> board_{};
};

}


#endif //RIGAMAGICIAN_BITBOARD_H