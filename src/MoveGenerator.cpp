#include "MoveGenerator.h"
#include <sstream>

namespace Rmagician {
void MoveGenerator::generate_moves(const Position &pos, MoveList &move_list) {
    move_list.count = 0;
    Color side = pos.side_to_move();

    generate_pawn_moves(pos, side, move_list);
    generate_knight_moves(pos, side, move_list);
    generate_slider_moves(pos, side, move_list);
    generate_king_moves(pos, side, move_list);
    generate_castling(pos, side, move_list);
}

void MoveGenerator::generate_pawn_moves(
    const Position& pos,
    Color side,
    MoveList& move_list
) {
    const Bitboards& b = pos.bitboards();
    Bitboard pawns = b.pieces(PAWN, side);
    Bitboard occupied = b.all();
    Bitboard enemies = b.pieces(side == WHITE ? BLACK : WHITE);

    Direction up = (side == WHITE) ? NORTH : SOUTH;
    Direction up_left = (side == WHITE) ? NORTH_WEST : SOUTH_EAST;
    Direction up_right = (side == WHITE) ? NORTH_EAST : SOUTH_WEST;

    Bitboard rank_3_mask = (side == WHITE) ? 0x0000000000FF0000ULL : 0x0000FF0000000000ULL;
    int promotion_rank = (side == WHITE) ? 7 : 0;

    Bitboard single_push = BitboardUtils::shift(pawns, up) & ~occupied;
    Bitboard double_push = BitboardUtils::shift(single_push & rank_3_mask, up) & ~occupied;

    Bitboard temp_single = single_push;
    while (temp_single) {
        Square to = BitboardUtils::pop_lsb(temp_single);
        Square from = static_cast<Square>(to - up);

        if (BitboardUtils::rank_of(to) == promotion_rank) {
            add_pawn_move(from, to, PROMOTION, move_list);
        } else {
            move_list.push(Move(from, to, PIECE_NUM, QUIET));
        }
    }

    while (double_push) {
        Square to = BitboardUtils::pop_lsb(double_push);
        Square from = static_cast<Square>(to - up - up);
        move_list.push(Move(from, to, PIECE_NUM, DOUBLE_PAWN_PUSH));
    }

    auto generate_captures = [&](Direction dir) {
        Bitboard attacks = BitboardUtils::shift(pawns, dir) & enemies;
        while (attacks) {
            Square to = BitboardUtils::pop_lsb(attacks);
            Square from = static_cast<Square>(to - dir);

            if (BitboardUtils::rank_of(to) == promotion_rank) {
                add_pawn_move(from, to, PROMOTION, move_list); // Promotion with capture
            } else {
                move_list.push(Move(from, to, PIECE_NUM, CAPTURE));
            }
        }
    };

    generate_captures(up_left);
    generate_captures(up_right);

    if (auto ep_sq = pos.en_passant_square()) {
        Square target = *ep_sq;
        Bitboard candidates = attacks_.get_pawn_attacks(target, side == WHITE ? BLACK : WHITE) & pawns;
        while (candidates) {
            Square from = static_cast<Square>(BitboardUtils::pop_lsb(candidates));
            move_list.push(Move(from, target, PIECE_NUM, EN_PASSANT));
        }
    }
}

void MoveGenerator::add_pawn_move(
    Square from,
    Square to,
    MoveFlag flag,
    MoveList& list
) {
    if (flag == PROMOTION) {
        const bool white_promo = BitboardUtils::rank_of(to) == 7;
        if (white_promo) {
            list.push(Move(from, to, W_QUEEN, PROMOTION));
            list.push(Move(from, to, W_ROOK, PROMOTION));
            list.push(Move(from, to, W_BISHOP, PROMOTION));
            list.push(Move(from, to, W_KNIGHT, PROMOTION));
        } else {
            list.push(Move(from, to, B_QUEEN, PROMOTION));
            list.push(Move(from, to, B_ROOK, PROMOTION));
            list.push(Move(from, to, B_BISHOP, PROMOTION));
            list.push(Move(from, to, B_KNIGHT, PROMOTION));
        }
    } else {
        list.push(Move(from, to, PIECE_NUM, flag));
    }
}

void MoveGenerator::generate_knight_moves(
    const Position& pos,
    Color side,
    MoveList& move_list
) {
    Bitboard knights = pos.bitboards().pieces(KNIGHT, side);
    Bitboard friends = pos.bitboards().pieces(side);
    Bitboard enemies = pos.bitboards().pieces(side == WHITE ? BLACK : WHITE);

    while (knights) {
        Square from = BitboardUtils::pop_lsb(knights);

        Bitboard knight_attacks = attacks_.get_knight_attacks(from) & ~friends;
        while (knight_attacks) {
            Square to = BitboardUtils::pop_lsb(knight_attacks);

            MoveFlag flag = (BitboardUtils::square_bb(to) & enemies) ? CAPTURE : QUIET;
            move_list.push(Move(from, to, PIECE_NUM, flag));
        }
    }
}

void MoveGenerator::generate_king_moves(
    const Position &pos,
    Color side,
    MoveList &move_list
) {
    Bitboard king = pos.bitboards().pieces(KING, side);
    if (!king) {
        return;
    }

    Bitboard friends = pos.bitboards().pieces(side);
    Bitboard enemies = pos.bitboards().pieces(side == WHITE ? BLACK : WHITE);

    Square from = BitboardUtils::pop_lsb(king);
    Bitboard king_attacks = attacks_.get_king_attacks(from) & ~friends;

    while (king_attacks) {
        Square to = BitboardUtils::pop_lsb(king_attacks);
        MoveFlag flag = (BitboardUtils::square_bb(to) & enemies) ? CAPTURE : QUIET;
        move_list.push(Move(from, to, PIECE_NUM, flag));
    }
}

void MoveGenerator::generate_slider_moves(
    const Position& pos,
    Color side,
    MoveList& move_list
) {
    Bitboard occupied = pos.bitboards().all();
    Bitboard friendly = pos.bitboards().pieces(side);
    Bitboard enemies = pos.bitboards().pieces(side == WHITE ? BLACK : WHITE);

    auto generate_for_type = [&](PieceType pt) {
        Bitboard pieces = pos.bitboards().pieces(pt, side);
        while (pieces) {
            Square from = BitboardUtils::pop_lsb(pieces);
            Bitboard moves_bb = 0;

            if (pt == ROOK || pt == QUEEN) {
                moves_bb |= attacks_.get_rook_attacks(from, occupied);
            }
            if (pt == BISHOP || pt == QUEEN) {
                moves_bb |= attacks_.get_bishop_attacks(from, occupied);
            }

            moves_bb &= ~friendly; // Can't capture friend pieces

            while (moves_bb) {
                Square to = BitboardUtils::pop_lsb(moves_bb);
                MoveFlag flag = (BitboardUtils::square_bb(to) & enemies) ? CAPTURE : QUIET;
                move_list.push(Move(from, to, PIECE_NUM, flag));
            }
        }
    };

    generate_for_type(ROOK);
    generate_for_type(BISHOP);
    generate_for_type(QUEEN);
}

void MoveGenerator::generate_castling(
    const Position& pos,
    Color side,
    MoveList& move_list
) {
    const uint8_t rights = pos.castling_rights();
    const Color enemy = (side == WHITE) ? BLACK : WHITE;
    const auto& board = pos.bitboards();

    if (side == WHITE) {
        if ((rights & WHITE_OO) &&
            board.piece_on(E1) == W_KING &&
            board.piece_on(H1) == W_ROOK &&
            board.piece_on(F1) == PIECE_NUM &&
            board.piece_on(G1) == PIECE_NUM &&
            !pos.is_square_attacked(E1, enemy) &&
            !pos.is_square_attacked(F1, enemy) &&
            !pos.is_square_attacked(G1, enemy)) {
            move_list.push(Move(E1, G1, PIECE_NUM, KING_CASTLE));
        }

        if ((rights & WHITE_OOO) &&
            board.piece_on(E1) == W_KING &&
            board.piece_on(A1) == W_ROOK &&
            board.piece_on(D1) == PIECE_NUM &&
            board.piece_on(C1) == PIECE_NUM &&
            board.piece_on(B1) == PIECE_NUM &&
            !pos.is_square_attacked(E1, enemy) &&
            !pos.is_square_attacked(D1, enemy) &&
            !pos.is_square_attacked(C1, enemy)) {
            move_list.push(Move(E1, C1, PIECE_NUM, QUEEN_CASTLE));
        }
    } else {
        if ((rights & BLACK_OO) &&
            board.piece_on(E8) == B_KING &&
            board.piece_on(H8) == B_ROOK &&
            board.piece_on(F8) == PIECE_NUM &&
            board.piece_on(G8) == PIECE_NUM &&
            !pos.is_square_attacked(E8, enemy) &&
            !pos.is_square_attacked(F8, enemy) &&
            !pos.is_square_attacked(G8, enemy)) {
            move_list.push(Move(E8, G8, PIECE_NUM, KING_CASTLE));
        }

        if ((rights & BLACK_OOO) &&
            board.piece_on(E8) == B_KING &&
            board.piece_on(A8) == B_ROOK &&
            board.piece_on(D8) == PIECE_NUM &&
            board.piece_on(C8) == PIECE_NUM &&
            board.piece_on(B8) == PIECE_NUM &&
            !pos.is_square_attacked(E8, enemy) &&
            !pos.is_square_attacked(D8, enemy) &&
            !pos.is_square_attacked(C8, enemy)) {
            move_list.push(Move(E8, C8, PIECE_NUM, QUEEN_CASTLE));
        }
    }
}

} // RMagician
