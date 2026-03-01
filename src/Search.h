#ifndef RIGAMAGICIAN_SEARCH_H
#define RIGAMAGICIAN_SEARCH_H

#include "Position.h"
#include "MoveGenerator.h"
#include "Evaluation.h"

///TODO: add Move Ordering and Transposition Table
namespace Rmagician {

    /**
     * @brief Alpha-beta searcher based on negamax with quiescence extension.
     * @note Negamax is the same minimax algorythm, but in another form.
     * It based on math equation max(a,b) = -min(-a,-b).
     * @see
     * https://www.geeksforgeeks.org/dsa/minimax-algorithm-in-game-theory-set-1-introduction/
     *
     */
    class Search {
    public:
        struct SearchParams {
            /**
             * @brief Maximum depth in binary search.
             */
            int depth;
        };

        /**
         * @brief Searches the current position and returns the best legal move found.
         * @param pos Mutable position used during search. The method makes and undoes moves.
         * @param params Search configuration values (for example, max depth).
         * @return Best move according to the evaluation and search window.
         */
        Move find_best_move(Position& pos, SearchParams params);

    private:
        /**
         * @brief Recursive negamax search with alpha-beta pruning.
         * @param pos Position at the current search node.
         * @param depth Remaining depth in plies. Exit when depth reaches 0.
         * @param alpha Lower bound of the current search window.
         * @param beta Upper bound of the current search window.
         * @return Best score found in the from side-to-move perspective.
         */
        int negamax(Position& pos, int depth, int alpha, int beta);

        /**
         * @brief Quiescence search that explores tactical continuations to reduce horizon effects.
         * @param pos Mutable position at the current quiescence node.
         * @param alpha Lower bound of the current search window.
         * @param beta Upper bound of the current search window.
         * @return Refined static score after searching capture/en-passant continuations.
         */
        int quiescence(Position& pos, int alpha, int beta);

        MoveGenerator generator_;
    };

} // namespace Rmagician

#endif
