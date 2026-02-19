#ifndef RIGAMAGICIAN_FEN_H
#define RIGAMAGICIAN_FEN_H
#include "../Bitboards.h"
#include "../Types.h"
#include <string>

namespace Rmagician {

class FenHelper {
public:
    /*!
     * @brief Parse FEN string and write resulting state into output arguments.
     * @param fen Input FEN.
     * @param bb Output board state.
     * @param pd Output non-board position details.
     * @return true on valid FEN, false otherwise.
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
    static bool parse_fen(const std::string& fen, Bitboards& bb, PositionDetails& pd);

    /*!
     * @brief Convert board + position details into a FEN string.
     * @param bb Board state.
     * @param pd Non-board position details.
     * @return Valid FEN string.
     */
    static std::string to_fen(const Bitboards& bb, const PositionDetails& pd);

private:
    /*!
     * @brief Parse pieces positions from FEN
     * @return true on success
     */
    static bool parse_piece_placement(const std::string& placement, Bitboards& bb);

    /*!
     * @brief Parse castling rights from FEN
     * @return true on success
     */
    static bool parse_castling_rights(const std::string& castling, PositionDetails& pd);

    /*!
     * @brief Parse en-passant square from FEN
     * @return true on success
     */
    static bool parse_en_passant(const std::string& ep, PositionDetails& pd);
};
} // Rmagician

#endif //RIGAMAGICIAN_FEN_H
