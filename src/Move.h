#ifndef RIGAMAGICIAN_MOVE_H
#define RIGAMAGICIAN_MOVE_H

#include "Types.h"

namespace Rmagician {
/*!
 * @class Move
 * @brief Compact 16-bit encoding of a chess move.
 *
 * @details
 * Move stores source square, destination square, and move flag in a single
 * `uint16_t` value:
 * - bits [0..5]   : source square (`from`)
 * - bits [6..11]  : destination square (`to`)
 * - bits [12,13]  : promotion piece type (KNIGHT, BISHOP, ROOK, QUEEN)
 * - bits [14..17] : move type (`MoveFlag`)
 * - bits [18..32] : unused
 *
 * bits:  | 32..18 | 17 16 15 14 | 13 12 | 11 10 9 8 7 6 | 5 4 3 2 1 0
 *          unused | flag        | prom  | to square     | from square
 *                                 piece
 *
 * This representation is efficient for move generation and search.
 */
class Move {
public:
    /*!
     * @brief Construct a move from source and destination squares.
     * @param from Source square.
     * @param to Destination square.
     * @param flag Move kind (quiet, capture, castling, promotion, etc.).
     *
     * @note How bit operation works:
     *  0x3F (hex) is 0b00111111, so (from & 0x3F) done to be sure that
     *  we get only first 6 bits
     */
    Move(Square from, Square to, MoveFlag flag = QUIET) {
        data_ = (from & 0x3F) | ((to & 0x3F) << 6) | (flag << 12);
    }

    /*!
     * @brief Get source square.
     * @return Source square from the encoded move.
     */
    Square from() const { return static_cast<Square>(data_ & 0x3F); }

    /*!
     * @brief Get destination square.
     * @return Destination square from the encoded move.
     */
    Square to() const { return static_cast<Square>((data_ >> 6) & 0x3F); }

    /*!
     * @brief Get move flag.
     * @return Encoded move type.
     */
    MoveFlag flag() const { return static_cast<MoveFlag>(data_ >> 14); }

    /*!
     * @brief Get piece pawn was turned into (by promotion)
     * @return Piece
     *
     * @note 0x3 (hex) is 0b0011 (binary), so (data_) & 0x3 done to
     * be sure that we take only 2 bits
     */
    Piece promotion_piece() const { return static_cast<Piece>((data_ >> 12) & 0x3); }

    /*!
     * @brief Check if move is empty.
     * @return true if move has zero payload.
     */
    bool is_none() const { return data_ == 0; }

private:
    uint32_t data_;
};

/*!
 * @struct UndoInfo
 * @brief Saves position state to have possibility to undo it
 */
struct UndoInfo {
    uint8_t castling_rights;
    std::optional<Square> en_passant_square;
    int halfmove_clock;
    Piece captured_piece = PIECE_NUM;
};

} // namespace Rmagician

#endif //RIGAMAGICIAN_MOVE_H
