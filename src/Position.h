#ifndef RIGAMAGICIAN_POSITION_H
#define RIGAMAGICIAN_POSITION_H

#include <string>
#include "Bitboards.h"
#include "Move.h"

namespace Rmagician {

/*!
 * @class Position
 * @brief Implements chess board's full state
 */
class Position {
public:
    Position() = default;

    /*!
     * @brief Initialize board with standard start position
     */
    void set_start_position();

    /*!
     * @brief Uploads position from FEN-notation
     * @param fen FEN-string
     * @return true on success, false otherwise
     */
    bool set_from_fen(const std::string& fen);

    /*!
     * @brief Converts current position into FEN-notation
     * @return FEN string
     */
    std::string to_fen() const;

    /*!
     * @brief Prints board for debug
     */
    void print() const;

    void make_move(Move& m, UndoInfo& undo);
    void undo_move(Move& m, UndoInfo& undo);

    const Bitboards& bitboards() const { return bitboards_; }
    Bitboards& bitboards() { return bitboards_; }

    /*!
     * @brief Check if there is king of specified color on board
     * @return true if king on board, false otherwise
     */
    bool has_king(Color c) const;

private:
    Bitboards bitboards_;
    PositionDetails pd_;

    void process_pawn_move(Move& m);
    void process_castling_rights(Move& m);
    void process_castling(Move& m);
};

} // namespace Rmagician

#endif //RIGAMAGICIAN_POSITION_H
