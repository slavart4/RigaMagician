#include "BitboardUtils.h"
#include <iostream>
#include <iomanip>

namespace Rmagician::BitboardUtils {

    void print_bitboard(Bitboard bb, const std::string& name) {
        if (!name.empty()) {
            std::cout << "\n" << name << ":\n";
        }

        std::cout << "\n  +---+---+---+---+---+---+---+---+\n";

        for (int rank = 7; rank >= 0; rank--) {
            std::cout << (rank + 1) << " |";

            for (int file = 0; file < 8; file++) {
                Square sq = make_square(file, rank);
                bool bit_set = (bb >> sq) & 1;
                std::cout << " " << (bit_set ? "X" : ".") << " |";
            }

            std::cout << "\n  +---+---+---+---+---+---+---+---+\n";
        }

        std::cout << "    a   b   c   d   e   f   g   h\n";
        std::cout << "\nBitboard: 0x" << std::hex << std::setw(16)
                  << std::setfill('0') << bb << std::dec << "\n";
        std::cout << "Popcount: " << popcount(bb) << "\n\n";
    }

}
