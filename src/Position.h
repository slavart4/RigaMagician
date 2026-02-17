#ifndef RIGAMAGICIAN_POSITION_H
#define RIGAMAGICIAN_POSITION_H

#include "Bitboards.h"
#include "Types.h"
#include <string>
#include <optional>

namespace Rmagician {

/*!
 * @class Position
 * @brief Implements chess board's full state
 * @details
 * Includes:
 * - Pieces positions (using Bitboards)
 * - Side to move
 * - Castling rights
 * - En passant square
 * - Counters for 50-move rule and move number
 */
class Position {
public:
    Position();

    /*!
     * @brief Initialize board with standard start position
     */
    void set_start_position();

    /*!
     * @brief Uploads position from FEN-notation
     * @param fen FEN-string
     * @return true on success, false otherwise
     *
     * @details FEN (Forsyth-Edwards Notation) has 6 parts:
     * 1. Pieces positions (8 raws separated by "/")
     * 2. Side to move (w or b)
     * 3. Castling rights (KQkq or -)
     * 4. En passant square (e3 or -)
     * 5. Halfmove clock (for the 50-move rule)
     * 6. Fullmove number (move number)
     *
     * Example: "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
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

    Color side_to_move()                        const { return side_to_move_; }
    uint8_t castling_rights()                   const { return castling_rights_; }
    std::optional<Square> en_passant_square()   const { return en_passant_square_; }
    int halfmove_clock()                        const { return halfmove_clock_; }
    int fullmove_number()                       const { return fullmove_number_; }

    const Bitboards& bitboards() const { return bitboards_; }
    Bitboards& bitboards() { return bitboards_; }

    /*!
     * @brief Check if there is king of specified color on board
     * @return true if king on board, false otherwise
     */
    bool has_king(Color c) const;

private:
    Bitboards bitboards_;

    Color side_to_move_;                        /*!< What side should move */
    uint8_t castling_rights_;                   /*!< Castling rights bitmask */
    std::optional<Square> en_passant_square_;   /*!< Square for en passant capture */
    int halfmove_clock_;                        /*!< Counter for 50-move rule */
    int fullmove_number_;                       /*!< Complete moves number (increments after black moves) */

    /*!
     * @brief Parse pieces positions from FEN
     * @return true on success
     */
    bool parse_piece_placement(const std::string& placement);

    /*!
     * @brief Parse castling rights from FEN
     * @return true on success
     */
    bool parse_castling_rights(const std::string& castling);

    /*!
     * @brief Parse en-passant square from FEN
     * @return true on success
     */
    bool parse_en_passant(const std::string& ep);
};

} // namespace Rmagician

#endif //RIGAMAGICIAN_POSITION_H