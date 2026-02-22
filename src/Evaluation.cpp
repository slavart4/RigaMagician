#include "Evaluation.h"
namespace Rmagician {
int Evaluation::evaluate(const Position &pos) {
    return evaluate_material(pos) + evaluate_psqt(pos) + evaluate_pawns(pos);
}

int Evaluation::evaluate_material(const Position& pos) {
    const auto& b = pos.bitboards();
    int score = 0;

    score += BitboardUtils::popcount(b.pieces(PAWN, WHITE)) * PAWN_VALUE;
    score -= BitboardUtils::popcount(b.pieces(PAWN, BLACK)) * PAWN_VALUE;

    score += BitboardUtils::popcount(b.pieces(ROOK, WHITE)) * ROOK_VALUE;
    score -= BitboardUtils::popcount(b.pieces(ROOK, BLACK)) * ROOK_VALUE;

    score += BitboardUtils::popcount(b.pieces(KNIGHT, WHITE)) * KNIGHT_VALUE;
    score -= BitboardUtils::popcount(b.pieces(KNIGHT, BLACK)) * KNIGHT_VALUE;

    score += BitboardUtils::popcount(b.pieces(BISHOP, WHITE)) * BISHOP_VALUE;
    score -= BitboardUtils::popcount(b.pieces(BISHOP, BLACK)) * BISHOP_VALUE;

    score += BitboardUtils::popcount(b.pieces(QUEEN, WHITE)) * QUEEN_VALUE;
    score -= BitboardUtils::popcount(b.pieces(QUEEN, BLACK)) * QUEEN_VALUE;

    score += BitboardUtils::popcount(b.pieces(KING, WHITE)) * KING_VALUE;
    score -= BitboardUtils::popcount(b.pieces(KING, BLACK)) * KING_VALUE;

    return score;
}

int Evaluation::evaluate_psqt(const Position &pos) {
    int score = 0;
    const auto& b = pos.bitboards();

    auto process_piece = [&](PieceType pt, const int table[]) {
        Bitboard white_pieces = b.pieces(pt, WHITE);
        while (white_pieces) {
            Square sq =BitboardUtils::pop_lsb(white_pieces);
            score += table[sq];
        }

        Bitboard black_pieces = b.pieces(pt, BLACK);
        while (black_pieces) {
            Square sq = BitboardUtils::pop_lsb(black_pieces);
            // Mirroring: Square(sq) ^ 56 reversing board for black
            score -= table[sq ^ 56];
        }
    };

    process_piece(PAWN, PSQT::Pawn);
    process_piece(KNIGHT, PSQT::Knight);
    process_piece(BISHOP, PSQT::Bishop);
    process_piece(ROOK, PSQT::Rook);
    process_piece(QUEEN, PSQT::Queen);

    return score;
}

int Evaluation::evaluate_pawns(const Position& pos) {
    int score = 0;
    const auto& b = pos.bitboards();

    Bitboard white_pawns = b.pieces(PAWN, WHITE);
    Bitboard black_pawns = b.pieces(PAWN, BLACK);

    auto get_file_mask = [](int file) {
        return 0x0101010101010101ULL << file;
    };

    auto get_adjacent_files_mask = [&](int file) {
        Bitboard mask = 0;
        if (file > 0) mask |= get_file_mask(file - 1);
        if (file < 7) mask |= get_file_mask(file + 1);
        return mask;
    };

    for (int file = 0; file < 8; file++) {
        Bitboard file_mask = get_file_mask(file);
        Bitboard adj_mask = get_adjacent_files_mask(file);

        Bitboard w_pawns_on_file = white_pawns & file_mask;
        if (w_pawns_on_file) {
            if (BitboardUtils::popcount(w_pawns_on_file) > 1)
                score += DOUBLED_PAWN_PENALTY;

            if (!(white_pawns & adj_mask))
                score += ISOLATED_PAWN_PENALTY;
        }

        Bitboard b_pawns_on_file = black_pawns & file_mask;
        if (b_pawns_on_file) {
            if (BitboardUtils::popcount(b_pawns_on_file) > 1)
                score -= DOUBLED_PAWN_PENALTY;

            if (!(black_pawns & adj_mask))
                score -= ISOLATED_PAWN_PENALTY;
        }
    }

    auto is_passed = [&](Square sq, Color c) {
        int file = BitboardUtils::file_of(sq);
        int rank = BitboardUtils::rank_of(sq);
        Bitboard mask = get_file_mask(file) | get_adjacent_files_mask(file);

        if (c == WHITE) {
            Bitboard forward_mask = 0xFFFFFFFFFFFFFF00ULL << (8 * rank);
            return !(black_pawns & mask & forward_mask);
        } else {
            Bitboard forward_mask = 0x00FFFFFFFFFFFFFFULL >> (8 * (7 - rank));
            return !(white_pawns & mask & forward_mask);
        }
    };

    Bitboard wp = white_pawns;
    while (wp) {
        Square sq = BitboardUtils::pop_lsb(wp);
        if (is_passed(sq, WHITE)) score += PASSED_PAWN_BONUS[BitboardUtils::rank_of(sq)];
    }

    Bitboard bp = black_pawns;
    while (bp) {
        Square sq = BitboardUtils::pop_lsb(bp);
        if (is_passed(sq, BLACK)) score -= PASSED_PAWN_BONUS[7 - BitboardUtils::rank_of(sq)];
    }

    return score;
}

}
