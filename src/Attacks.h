#ifndef RIGAMAGICIAN_ATTACKS_H
#define RIGAMAGICIAN_ATTACKS_H

#include "utils/BitboardUtils.h"

/// TODO: for now we find pieces possible attacks "on fly"
///  but better rewrite to magic bitmasks
///  (precalculated attacks for all squares and pieces)

namespace Rmagician {

/*!
 * @class Attacks
 * @brief Attack generation helper for chess pieces.
 */
class Attacks {
public:
    /*!
     * @brief Initializes precomputed attack tables.
     *
     * @details
     * Must be called once before querying leaper attacks.
     * Fills internal tables with attacks for all 64 squares.
     */
    void init();

    /*!
     * @brief Returns rook attacks from a square.
     * @param sq Source square.
     * @param occupied Bitboard of all occupied squares.
     * @return Bitboard of attacked squares until first blocker
     * in each rook direction.
     */
    Bitboard get_rook_attacks(Square sq, Bitboard occupied);

    /*!
     * @brief Returns bishop attacks from a square.
     * @param sq Source square.
     * @param occupied Bitboard of all occupied squares.
     * @return Bitboard of attacked squares until first blocker
     * in each bishop direction.
     */
    Bitboard get_bishop_attacks(Square sq, Bitboard occupied);

    /*!
     * @brief Returns queen attacks from a square.
     * @param sq Source square.
     * @param occupied Bitboard of all occupied squares.
     * @return Union of rook and bishop attacks from the source square.
     */
    Bitboard get_queen_attacks(Square sq, Bitboard occupied);

    /*!
     * @brief Returns knight attacks from a square.
     * @param sq Source square.
     * @return Precomputed knight attack bitboard.
     */
    Bitboard get_knight_attacks(Square sq);

    /*!
     * @brief Returns kings attacks from a square.
     * @param sq Source square.
     * @return Precomputed king attack bitboard.
     *
     * @note Returns pseudo-legal king moves (can be moved on attacked square)
     */
    Bitboard get_king_attacks(Square sq);
private:
    /*!
     * @brief Traces attacks in one direction until the edge or blocker.
     * @param sq Source square.
     * @param dir Ray direction.
     * @param occupied Bitboard of occupied squares.
     * @return Bitboard of squares attacked in some direction.
     */
    Bitboard ray_attack(Square sq, Direction dir, Bitboard occupied);

    /*! Precomputed knight attacks for each square [0..63]. */
    Bitboard knight_attacks_[64] = {};

    /*! Precomputed king attacks for each square [0..63]. */
    Bitboard king_attacks_[64] = {};

    /*
     * Harder to deal with bcs this pieces cannot jump over other pieces.
     * So we have to deal with obstacles and store all possible variants.
     * Maybe do it later, for now just calculate attack.
     */
    // Bitboard bishop_attacks_[64];
    // Bitboard queen_attacks_[64];
    // Bitboard rook_attacks_[64];
    // Bitboard pawn_attacks_[COLOR_NUM][64];

}; // class Attacks

} // namespace Rmagician

#endif //RIGAMAGICIAN_ATTACKS_H
