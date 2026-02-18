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
 * - bits [12..15] : move type (`MoveFlag`)
 *
 * bits:  15 14 13 12 | 11 10 9 8 7 6 | 5 4 3 2 1 0
 *        flag        | to square     | from square
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
    MoveFlag flag() const { return static_cast<MoveFlag>(data_ >> 12); }

    /*!
     * @brief Check if move is empty.
     * @return true if move has zero payload.
     */
    bool is_none() const { return data_ == 0; }

private:
    uint16_t data_;
};

} // namespace Rmagician

#endif //RIGAMAGICIAN_MOVE_H
