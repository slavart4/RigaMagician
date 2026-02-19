#ifndef RIGAMAGICIAN_TYPES_H
#define RIGAMAGICIAN_TYPES_H
#include <iostream>
#include <optional>
#include <unordered_map>

namespace Rmagician {

using Bitboard = uint64_t;

enum Color : uint8_t {
    WHITE,
    BLACK,
    COLOR_NUM
};

enum PieceType : uint8_t {
    PAWN,
    ROOK,
    KNIGHT,
    BISHOP,
    QUEEN,
    KING,
    PIECE_TYPE_NUM
};

enum Piece : int {
    W_PAWN, W_KNIGHT, W_BISHOP, W_ROOK, W_QUEEN, W_KING,
    B_PAWN, B_KNIGHT, B_BISHOP, B_ROOK, B_QUEEN, B_KING,
    PIECE_NUM
};

enum Square : uint8_t {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8,
    SQUARE_NUM
};

/*!
 * @enum CastlingRights
 * @brief Bitmask to represent castling rights
 * @details Usage example:
 * @code
 * uint8_t castling_rights = NO_CASTLING; // 0b00000000
 *
 * // Add right for short castle for white
 * castling_rights |= WHITE_OO; // 0b00000000 |= 0b00000001 -> 0b00000001
 *
 * // Add right for long castle for black
 * castling_rights |= BLACK_OOO; // 0b00000001 |= 0b00001000 -> 0b00001001
 * @endcode
 * So 0b00001001 -> both rights are active
 */
enum CastlingRights : uint8_t {
    NO_CASTLING  = 0,       // 0b00000000
    WHITE_OO     = 1 << 0,  // 0b00000001
    WHITE_OOO    = 1 << 1,  // 0b00000010
    BLACK_OO     = 1 << 2,  // 0b00000100
    BLACK_OOO    = 1 << 3   // 0b00001000
};

/*!
 * @enum Direction
 * @brief Represents bit shift to move on some direction
 */
enum Direction {
    NORTH = 8,          /*!< Up */
    SOUTH = -8,         /*!< Down */
    EAST = 1,           /*!< Right */
    WEST = -1,          /*!< Left */
    NORTH_EAST = 9,     /*!< Diagonal up-right */
    NORTH_WEST = 7,     /*!< Diagonal up-left */
    SOUTH_EAST = -7,    /*!< Diagonal down-right */
    SOUTH_WEST = -9     /*!< Diagonal down-left */
};

// /*!
//  * @enum MoveFlag
//  * @brief Represents all possible move types
//  */
// enum MoveFlag : uint16_t {
//     NORMAL,
//     PROMOTION  = 1 << 14,
//     EN_PASSANT = 2 << 14,
//     CASTLING   = 3 << 14
// };

/*!
 * @enum MoveFlag
 * @brief Represents all possible move types
 */
enum MoveFlag : uint8_t {
    QUIET,              /*!< Simple piece move */
    CAPTURE,            /*!< Capture other piece */
    DOUBLE_PAWN_PUSH,   /*!< Double push from pawns start position */
    KING_CASTLE,        /*!< Castle to the short side */
    QUEEN_CASTLE,       /*!< Castle to the long side */
    EN_PASSANT,         /*!< En passant capture */
    PROMOTION           /*!< Turning pawn into other piece */
};

/*!
 * @struct PositionDetails
 * @brief Non-board state required to fully describe a chess position.
 */
struct PositionDetails {
    Color side_to_move {WHITE};                             /*!< What side should move */
    uint8_t castling_rights {NO_CASTLING};                  /*!< Castling rights bitmask */
    std::optional<Square> en_passant_square {std::nullopt}; /*!< Square for en passant capture */
    int halfmove_clock {0};                                 /*!< Counter for 50-move rule */
    int fullmove_number {1};                                /*!< Complete moves number (increments after black moves) */
};

static std::unordered_map<Piece, char> pieces_notations {
    {W_PAWN, 'P'}, {W_KNIGHT, 'N'}, {W_BISHOP, 'B'},
    {W_ROOK, 'R'}, {W_QUEEN, 'Q'}, {W_KING, 'K'},
    {B_PAWN, 'p'}, {B_KNIGHT, 'n'}, {B_BISHOP, 'b'},
    {B_ROOK, 'r'}, {B_QUEEN, 'q'}, {B_KING, 'k'},
    {PIECE_NUM, ' '}
};

static std::unordered_map<char, Piece> notations_pieces {
    {'P', W_PAWN}, {'N', W_KNIGHT}, {'B', W_BISHOP},
    {'R', W_ROOK}, {'Q', W_QUEEN}, {'K', W_KING},
    {'p', B_PAWN}, {'n', B_KNIGHT}, {'b', B_BISHOP},
    {'r', B_ROOK}, {'q', B_QUEEN}, {'k', B_KING}
};

}


#endif //RIGAMAGICIAN_TYPES_H
