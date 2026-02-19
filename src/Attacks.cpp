#include "Attacks.h"

namespace Rmagician {

void Attacks::init() {
    for (int s = 0; s < 64; ++s) {
        Bitboard b = 1ULL << s;

        knight_attacks_[s] =
            BitboardUtils::shift(b, static_cast<Direction>(NORTH+NORTH_EAST)) |
            BitboardUtils::shift(b, static_cast<Direction>(NORTH+NORTH_WEST)) |
            BitboardUtils::shift(b, static_cast<Direction>(SOUTH+SOUTH_EAST)) |
            BitboardUtils::shift(b, static_cast<Direction>(SOUTH+SOUTH_WEST)) |
            BitboardUtils::shift(b, static_cast<Direction>(EAST+NORTH_EAST))  |
            BitboardUtils::shift(b, static_cast<Direction>(EAST+SOUTH_EAST))  |
            BitboardUtils::shift(b, static_cast<Direction>(WEST+NORTH_WEST))  |
            BitboardUtils::shift(b, static_cast<Direction>(WEST+SOUTH_WEST));

        king_attacks_[s] =
            BitboardUtils::shift(b, NORTH) |
            BitboardUtils::shift(b, SOUTH) |
            BitboardUtils::shift(b, EAST)  |
            BitboardUtils::shift(b, WEST)  |
            BitboardUtils::shift(b, NORTH_EAST) |
            BitboardUtils::shift(b, NORTH_WEST) |
            BitboardUtils::shift(b, SOUTH_EAST) |
            BitboardUtils::shift(b, SOUTH_WEST);
    }
}

Bitboard Attacks::ray_attack(Square sq, Direction dir, Bitboard occupied) const {
    Bitboard attacks = 0ULL;
    int rank = sq / 8;
    int file = sq % 8;

    int dr = (dir == NORTH || dir == NORTH_EAST || dir == NORTH_WEST) ? 1 :
             (dir == SOUTH || dir == SOUTH_EAST || dir == SOUTH_WEST) ? -1 : 0;
    int df = (dir == EAST || dir == NORTH_EAST || dir == SOUTH_EAST) ? 1 :
             (dir == WEST || dir == NORTH_WEST || dir == SOUTH_WEST) ? -1 : 0;

    int cur_rank = rank + dr;
    int cur_file = file + df;

    while (cur_rank >= 0 && cur_rank < 8 && cur_file >= 0 && cur_file < 8) {
        Bitboard bit = 1ULL << (cur_rank * 8 + cur_file);
        attacks |= bit;
        if (occupied & bit) break;
        cur_rank += dr;
        cur_file += df;
    }
    return attacks;
}

Bitboard Attacks::get_pawn_attacks(Square sq, Color side) const {
    Bitboard b = 1ULL << sq;
    if (side == WHITE) {
        return BitboardUtils::shift(b, NORTH_EAST) |
               BitboardUtils::shift(b, NORTH_WEST);
    }

    return BitboardUtils::shift(b, SOUTH_EAST) |
           BitboardUtils::shift(b, SOUTH_WEST);
}

Bitboard Attacks::get_rook_attacks(Square sq, Bitboard occupied) const {
    return
        ray_attack(sq, NORTH, occupied) |
        ray_attack(sq, SOUTH, occupied) |
        ray_attack(sq, EAST, occupied)  |
        ray_attack(sq, WEST, occupied);
}

Bitboard Attacks::get_bishop_attacks(Square sq, Bitboard occupied) const {
    return
        ray_attack(sq, NORTH_EAST, occupied) |
        ray_attack(sq, NORTH_WEST, occupied) |
        ray_attack(sq, SOUTH_EAST, occupied) |
        ray_attack(sq, SOUTH_WEST, occupied);
}

Bitboard Attacks::get_queen_attacks(Square sq, Bitboard occupied) const {
    return get_rook_attacks(sq, occupied) |
           get_bishop_attacks(sq, occupied);
}

Bitboard Attacks::get_knight_attacks(Square sq) const {
    return knight_attacks_[sq];
}
Bitboard Attacks::get_king_attacks(Square sq) const {
    return king_attacks_[sq];
}

}
