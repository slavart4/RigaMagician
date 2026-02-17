#ifndef RIGAMAGICIAN_BITBOARD_UTILS_H
#define RIGAMAGICIAN_BITBOARD_UTILS_H

#include "Types.h"
#include <string>

namespace Rmagician {

/*!
 * @brief Utility functions for working with Bitboards
 *
 * These functions use CPU intrinsics for fast bitwise operations.
 * Modern compilers (GCC/Clang/MSVC) automatically optimize them
 * into processor instructions.
 *
 * All methods should work extremely fast, so they are inline to
 * avoid CPU registers saving, RET calls etc.
 */
namespace BitboardUtils {

    /*!
     * @brief Counts the number of set bits (population count)
     * @param bb Bitboard
     * @return Number of ones in the bitboard
     *
     * @details Uses compiler intrinsic functions:
     * - GCC/Clang: __builtin_popcountll
     * - MSVC: __popcnt64
     *
     * Example: popcount(0b00001101) = 3
     */
    inline int popcount(Bitboard bb) {
        #ifdef _MSC_VER
            return static_cast<int>(__popcnt64(bb));
        #else
            return __builtin_popcountll(bb);
        #endif
    }

    /*!
     * @brief Finds the index of the least significant set bit (LSB)
     * @param bb Bitboard (must be non-zero!)
     * @return Index of LSB (0-63)
     *
     * @details Uses:
     * - GCC/Clang: __builtin_ctzll (count trailing zeros)
     * - MSVC: _BitScanForward64
     *
     * Example: lsb(0b00101000) = 3 (bit at position 3).
     *
     * @warning Undefined behavior if bb == 0!
     */
    inline int lsb(Bitboard bb) {
        #ifdef _MSC_VER
            unsigned long idx;
            _BitScanForward64(&idx, bb);
            return static_cast<int>(idx);
        #else
            return __builtin_ctzll(bb);
        #endif
    }

    /*!
     * @brief Finds the index of the most significant set bit (MSB)
     * @param bb Bitboard (must be non-zero!)
     * @return Index of MSB (0-63)
     *
     * @details Uses:
     * - GCC/Clang: 63 - __builtin_clzll (count leading zeros)
     * - MSVC: _BitScanReverse64
     *
     * Example: msb(0b00101000) = 5 (bit at position 5)
     *
     * @warning Undefined behavior if bb == 0!
     */
    inline int msb(Bitboard bb) {
        #ifdef _MSC_VER
            unsigned long idx;
            _BitScanReverse64(&idx, bb);
            return static_cast<int>(idx);
        #else
            return 63 - __builtin_clzll(bb);
        #endif
    }

    /*!
     * @brief Removes the LSB from bitboard (pop LSB)
     * @param bb Reference to bitboard (non-const, will be modified!)
     * @return Square index of the popped LSB
     *
     * @details Use for iterating through set bits in a bitboard.
     *
     * Example usage:
     * @code
     * Bitboard pieces = 0b00101010;
     * while (pieces) {
     *     Square sq = pop_lsb(pieces);
     * }
     * @endcode
     *
     * @note How bit operations works
     *  00101000 - 1 -> 00100111
     *  00101000 &= 00100111 -> 00100000
     *  So we removed least significant set bit (LSB)
     */
    inline Square pop_lsb(Bitboard& bb) {
        Square sq = static_cast<Square>(lsb(bb));
        bb &= bb - 1;  // Clear the LSB: classic bit trick
        return sq;
    }

    /*!
     * @brief Checks if exactly one bit is set
     * @param bb Bitboard
     * @return true if only one bit is set, false otherwise
     *
     * @note How bit magic works
     * If not single bit:
     *  00101000 - 1 -> 00100111
     *  00101000 & 00100111 -> 00100000
     *  !(00100000) -> false
     *  00101000 (true) && 11011111 (false) -> false
     */
    inline bool is_single_bit(Bitboard bb) {
        return bb && !(bb & (bb - 1));
    }

    /*!
     * @brief Checks if more than one bit is set
     * @param bb Bitboard
     * @return true if multiple bits are set
     */
    inline bool has_multiple_bits(Bitboard bb) {
        return bb && (bb & (bb - 1));
    }

    /*!
     * @brief Converts square to bitboard mask
     * @param sq Square
     * @return Bitboard with single bit set at square position
     *
     * Example: square_bb(E4) = 0x0000001000000000
     */
    inline Bitboard square_bb(Square sq) {
        return 1ULL << sq;
    }

    /*!
     * @brief Combines two squares into a bitboard
     * @param sq1 First square
     * @param sq2 Second square
     * @return Bitboard with two bits set
     */
    inline Bitboard square_bb(Square sq1, Square sq2) {
        return square_bb(sq1) | square_bb(sq2);
    }

    /*!
     * @brief Returns the file (column) of a square
     * @param sq Square
     * @return File index (0=A, 1=B, ..., 7=H)
     *
     * @note How bit operations works:
     *  D2 square is 11 in Square enum (0b00001011)
     *  7 (0b00000111)
     *  0b00001011 & 0b00000111 -> 0b00000011 (3) - D file
     */
    inline int file_of(Square sq) {
        return sq & 7;  // Same as sq % 8
    }

    /*!
     * @brief Returns the rank (row) of a square
     * @param sq Square
     * @return Rank index (0=1st rank, 1=2nd rank, ..., 7=8th rank)
     *
     * @note How bit operations works:
     *  D2 square is 11 in Square enum (0b00001011)
     *  0b00001011 >> 3 -> 0b00000001 (1) - Second raw
     */
    inline int rank_of(Square sq) {
        return sq >> 3;  // Same as sq / 8
    }

    /*!
     * @brief Creates square from file and rank
     * @param file File (0-7)
     * @param rank Rank (0-7)
     * @return Square
     */
    inline Square make_square(int file, int rank) {
        return static_cast<Square>(rank * 8 + file);
    }

    /*!
     * @brief Converts square to algebraic notation (e.g., "e4")
     * @param sq Square
     * @return String representation
     */
    inline std::string square_to_string(Square sq) {
        char file = 'a' + file_of(sq); // int to char letters convert ascii trick
        char rank = '1' + rank_of(sq); // int to char numbers convert ascii trick
        return std::string(1, file) + std::string(1, rank);
    }

    /*!
     * @brief Converts algebraic notation to square (e.g., "e4" -> E4 from Squares enum)
     * @param str Notation (like "e4")
     * @return Square, or SQUARE_NUM if invalid
     */
    inline Square string_to_square(const std::string& str) {
        if (str.length() != 2) return SQUARE_NUM;

        int file = str[0] - 'a';
        int rank = str[1] - '1';

        if (file < 0 || file > 7 || rank < 0 || rank > 7) {
            return SQUARE_NUM;
        }

        return make_square(file, rank);
    }

    /*!
     * @brief Prints bitboard in a visual format (for debugging)
     * @param bb Bitboard
     * @param name Optional name to display
     */
    void print_bitboard(Bitboard bb, const std::string& name = "");

    /*!
     * @brief Shifts bitboard in a given direction
     * @param bb Bitboard
     * @param dir Shift direction
     * @return Shifted bitboard
     *
     * @details Useful for pawn moves and attacks
     * @note How bit operation works
     *  All squares on H file:
     *  0x8080808080808080ULL - in hex format
     *  0b1000000010000000100000001000000010000000100000001000000010000000 - in binary
     */
    inline Bitboard shift(Bitboard bb, Direction dir) {
        if (dir > 0) {
            if (dir == EAST || dir == NORTH_EAST || dir == SOUTH_EAST) {
                bb &= ~0x8080808080808080ULL; // Clear H-file
            }
            return bb << dir;
        }
        if (dir == WEST || dir == NORTH_WEST || dir == SOUTH_WEST) {
            bb &= ~0x0101010101010101ULL; // Clear A-file
        }
        return bb >> (-dir);
    }

} // namespace BitboardUtils

} // namespace Rmagician

#endif //RIGAMAGICIAN_BITBOARD_UTILS_H
