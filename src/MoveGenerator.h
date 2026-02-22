#ifndef RIGAMAGICIAN_MOVEGENERATOR_H
#define RIGAMAGICIAN_MOVEGENERATOR_H

#include "Move.h"
#include "Position.h"
#include "Bitboards.h"

/// TODO: add better documentation. Explain that we generate pseudo legal moves
///  because its cheaper to check it legality after we made move
///  (bcs of Alpha–beta pruning)

namespace Rmagician {

struct MoveList {
    Move moves[256]; // Максимальна кількість ходів у позиції рідко перевищує 100
    int count = 0;

    void push(Move m) { moves[count++] = m; }
};

class MoveGenerator {
public:
    MoveGenerator() { attacks_.init(); }

    /*!
     * @brief Main class method that returns all legal moves.
     * @param pos Current position.
     * @param move_list List with all possible moves in current position.
     */
    void generate_moves(
        const Position& pos,
        MoveList& move_list
    );

private:
    /*!
     * @brief Generates all pawns moves.
     * @param pos Current position.
     * @param side Color to move.
     * @param move_list List with all possible moves in position.
     */
    void generate_pawn_moves(
        const Position& pos,
        Color side,
        MoveList& move_list
    );

    /*!
     * @brief Generates all knights moves.
     * @param pos Current position.
     * @param side Color to move.
     * @param move_list List with all possible moves in position.
     */
    void generate_knight_moves(
        const Position& pos,
        Color side,
        MoveList& move_list
    );

    /*!
     * @brief Generates all pseudo-legal king moves.
     * @param pos Current position.
     * @param side Color to move.
     * @param move_list List with all possible moves in position.
     */
    void generate_king_moves(
        const Position& pos,
        Color side,
        MoveList& move_list
    );

    /*!
     * @brief Generates all sliding pieces moves.
     * @param pos Current position.
     * @param side Color to move.
     * @param move_list List with all possible moves in position.
     *
     * @details Sliding pieces are bishops, rooks and queens
     */
    void generate_slider_moves(
        const Position& pos,
        Color side,
        MoveList& move_list
    );

    /*!
     * @brief Generates all castling moves.
     * @param pos Current position.
     * @param side Color to move.
     * @param move_list List with all possible moves in position.
     */
    void generate_castling(
        const Position& pos,
        Color side,
        MoveList& move_list
    );

    // Helper method for adding moves into list (handling pawns promotions)
    void add_pawn_move(
        Square from,
        Square to,
        MoveFlag flag,
        MoveList& list
    );

    Attacks attacks_;
};

} // RMagician

#endif //RIGAMAGICIAN_MOVEGENERATOR_H
