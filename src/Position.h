#ifndef RIGAMAGICIAN_POSITION_H
#define RIGAMAGICIAN_POSITION_H

#include <string>
#include "Bitboards.h"
#include "Move.h"
#include "Attacks.h"

namespace Rmagician {

/*!
 * @class Position
 * @brief Implements chess board's full state.
 */
class Position {
public:
    Position() = default;

    /*!
     * @brief Initialize board with standard start position.
     */
    void set_start_position();

    /*!
     * @brief Uploads position from FEN-notation.
     * @param fen FEN-string.
     * @return true on success, false otherwise.
     */
    bool set_from_fen(const std::string& fen);

    /*!
     * @brief Converts current position into FEN-notation.
     * @return FEN string.
     */
    std::string to_fen() const;

    /*!
     * @brief Prints board for debug.
     */
    void print() const;

    void make_move(Move& m, UndoInfo& undo);
    void undo_move(Move& m, UndoInfo& undo);

    const Bitboards& bitboards() const { return bitboards_; }
    Bitboards& bitboards() { return bitboards_; }

    /*!
     * @brief Check if there is king of specified color on board.
     * @return true if king on board, false otherwise.
     */
    bool has_king(Color c) const;

    /*!
     * @brief Checks if square is under attack by pieces of some color.
     * @param sq Square to check.
     * @param attacker_color Color of the attacking pieces.
     * @return true if square under attack, false otherwise.
     *
     * @details Checks:
     *  1. Attacks of pawns of some color
     *  2. Knights attacks
     *  3. King attacks (for a check legality of king moves)
     *  4. Bishops and queen diagonal attacks
     *  5. Rooks and queen linear attacks
     */
    bool is_square_attacked(Square sq, Color attacker_color) const;

    /*!
     * @brief Method that returns castling rights on current position
     * @return uint8_t mask of castling rights
     */
    uint8_t castling_rights() const { return pd_.castling_rights; }

    /*!
     * @brief Method that returns en passant on current position
     * @return std::optional<Square>
     */
    std::optional<Square> en_passant_square() const { return pd_.en_passant_square; }

    /*!
     * @brief Return side that moves in current position
     * @return Color of moving side
     */
    Color side_to_move() const { return pd_.side_to_move;  }
private:
    Bitboards bitboards_;
    PositionDetails pd_;
    Attacks attacks_;

    void process_pawn_move(Move& m);
    void process_castling_rights(Move& m);
    void process_castling(Move& m);
};

} // namespace Rmagician

#endif //RIGAMAGICIAN_POSITION_H
