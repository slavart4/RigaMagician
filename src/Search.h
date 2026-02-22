#ifndef RIGAMAGICIAN_SEARCH_H
#define RIGAMAGICIAN_SEARCH_H

#include "Position.h"
#include "MoveGenerator.h"
#include "Evaluation.h"

///TODO: add Move Ordering and Transposition Table
namespace Rmagician {

    class Search {
    public:
        struct SearchParams {
            int depth;
        };

        /*!
         * @brief Main method, which returns best found move
         * @param pos Position
         * @param params SearchParams
         * @return The best found move
         */
        Move find_best_move(Position& pos, SearchParams params);

    private:
        /*!
         * @brief The main recursive searching algorythm
         * @param pos
         * @param depth
         * @param alpha
         * @param beta
         * @return
         */
        int negamax(Position& pos, int depth, int alpha, int beta);

        /*!
         *
         * @param pos
         * @param alpha
         * @param beta
         * @return
         */
        int quiescence(Position& pos, int alpha, int beta);

        MoveGenerator generator_;
    };

} // namespace Rmagician

#endif