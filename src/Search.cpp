#include "Search.h"
#include <algorithm>

namespace Rmagician {

constexpr int INF = 30000;
constexpr int MATE = 29000;

Move Search::find_best_move(Position& pos, SearchParams params) {
    Move best_move_found;
    MoveList list;
    generator_.generate_moves(pos, list);

    int best_score = -INF;
    int alpha = -INF;
    int beta = INF;

    for (int i = 0; i < list.count; i++) {
        UndoInfo undo;
        pos.make_move(list.moves[i], undo);
        const Color us = pos.side_to_move();
        const Color them = (us == WHITE) ? BLACK : WHITE;

        // If king under attack after move - move is illegal
        if (pos.is_square_attacked(pos.king_square(them), us)) {
            pos.undo_move(list.moves[i], undo);
            continue;
        }

        // Call negamax for the next level
        int score = -negamax(pos, params.depth - 1, -beta, -alpha);
        pos.undo_move(list.moves[i], undo);

        if (score > best_score) {
            best_score = score;
            best_move_found = list.moves[i];
        }
        alpha = std::max(alpha, score);
    }

    return best_move_found;
}

int Search::negamax(Position& pos, int depth, int alpha, int beta) {
    if (depth <= 0) {
        return quiescence(pos, alpha, beta);
    }

    MoveList list;
    generator_.generate_moves(pos, list);

    int legal_moves = 0;
    int best_score = -INF;

    for (int i = 0; i < list.count; i++) {
        UndoInfo undo;
        pos.make_move(list.moves[i], undo);
        const Color us = pos.side_to_move();
        const Color them = (us == WHITE) ? BLACK : WHITE;

        if (pos.is_square_attacked(pos.king_square(them), us)) {
            pos.undo_move(list.moves[i], undo);
            continue;
        }

        legal_moves++;
        int score = -negamax(pos, depth - 1, -beta, -alpha);
        pos.undo_move(list.moves[i], undo);

        if (score >= beta) return beta; // Alpha-Beta Cutoff
        if (score > alpha) alpha = score;
    }

    // Process checkmate and stalemate
    if (legal_moves == 0) {
        // If there is no moves and king is under attack - its checkmate
        const Color us = pos.side_to_move();
        const Color them = (us == WHITE) ? BLACK : WHITE;
        if (pos.is_square_attacked(pos.king_square(us), them)) {
            // Add depth for the fastest checkmate search
            return -MATE - depth;
        }
        // Stalemate
        return 0;
    }

    return alpha;
}

int Search::quiescence(Position& pos, int alpha, int beta) {
    int stand_pat = Evaluation::evaluate(pos);
    if (stand_pat >= beta) return beta;
    if (alpha < stand_pat) alpha = stand_pat;

    MoveList list;
    generator_.generate_moves(pos, list);

    for (int i = 0; i < list.count; i++) {
        if (list.moves[i].flag() != CAPTURE && list.moves[i].flag() != EN_PASSANT) continue;

        UndoInfo undo;
        pos.make_move(list.moves[i], undo);
        const Color us = pos.side_to_move();
        const Color them = (us == WHITE) ? BLACK : WHITE;
        if (pos.is_square_attacked(pos.king_square(them), us)) {
            pos.undo_move(list.moves[i], undo);
            continue;
        }

        int score = -quiescence(pos, -beta, -alpha);
        pos.undo_move(list.moves[i], undo);

        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }

    return alpha;
}

} // namespace Rmagician
