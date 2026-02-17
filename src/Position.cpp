#include "Position.h"
#include <sstream>
#include <vector>

namespace Rmagician {

Position::Position()
    : side_to_move_(WHITE)
    , castling_rights_(NO_CASTLING)
    , en_passant_square_(std::nullopt)
    , halfmove_clock_(0)
    , fullmove_number_(1) {}

void Position::set_start_position() {
    // Standard start position in FEN notation
    (void)set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

/*!
 * @details Split standard start position has this parts:
 *   1. "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR" - pieces positions
 *   2. "w" -        side to move
 *   3. "KQkq" -     castling rights
 *                    K - white can castle to the king side
 *                    Q - white can castle to the queen side
 *                    k - black can castle to the king side
 *                    q - black can castle to the queen side
 *   4. "-" -        en passant square (empty for the start position)
 *   5. "0" -        halfmove clock
 *   6. "1" -        fullmove clock
 *
 * @see https://www.chess.com/terms/fen-chess
 */
bool Position::set_from_fen(const std::string& fen) {
    bitboards_ = Bitboards();

    // split FEN into parts
    std::istringstream iss(fen);
    std::vector<std::string> parts;
    std::string part;

    while (iss >> part) {
        parts.push_back(part);
    }

    // FEN should have at 6 parts (or at least 4)
    if (parts.size() < 4) {
        return false;
    }

    // 1. Parse pieces
    if (!parse_piece_placement(parts[0])) {
        return false;
    }

    // 2. Parse side to move
    if (parts[1] == "w") {
        side_to_move_ = WHITE;
    } else if (parts[1] == "b") {
        side_to_move_ = BLACK;
    } else {
        return false;
    }

    // 3. Castling rights
    if (!parse_castling_rights(parts[2])) {
        return false;
    }

    // 4. En passant
    if (!parse_en_passant(parts[3])) {
        return false;
    }

    // 5. Halfmove clock (optional)
    if (parts.size() > 4) {
        try {
            halfmove_clock_ = std::stoi(parts[4]);
        } catch (...) {
            halfmove_clock_ = 0;
        }
    } else {
        halfmove_clock_ = 0;
    }

    // 6. Fullmove number (optional)
    if (parts.size() > 5) {
        try {
            fullmove_number_ = std::stoi(parts[5]);
        } catch (...) {
            fullmove_number_ = 1;
        }
    } else {
        fullmove_number_ = 1;
    }

    return true;
}

/*!
 * @note
 * FEN describes board from top to bottom (8 rank -> 1 rank), but
 * Square enum from Types goes from bottom to top (A1=0, A8=56)
 */
bool Position::parse_piece_placement(const std::string& placement) {
    int rank = 7; // Starts from 8 rank (rank 7 by index)
    int file = 0; // Starts from "A" column

    for (char c : placement) {
        if (c == '/') {
            rank--; // Goes to the next rank
            file = 0;
            if (rank < 0) {
                return false;
            }
        } else if (std::isdigit(c)) {
            // Digits describes number of empty squares
            int empty_squares = c - '0'; // ascii trick to convert char into int
            file += empty_squares;
            if (file > 8) {
                return false;
            }
        } else { // Piece symbol
            if (file >= 8) {
                return false;
            }

            Square sq = static_cast<Square>(rank * 8 + file);

            if (!notations_pieces.contains(c)) return false;
            Piece piece = notations_pieces[c]; // define piece by notation

            bitboards_.set_piece(sq, piece);
            file++;
        }
    }

    return rank == 0 && file == 8;  // Check if we filled whole board
}

bool Position::parse_castling_rights(const std::string& castling) {
    castling_rights_ = NO_CASTLING;

    if (castling == "-") {
        return true;  // No castling rights
    }

    for (char c : castling) {
        switch (c) {
            case 'K': castling_rights_ |= WHITE_OO; break;
            case 'Q': castling_rights_ |= WHITE_OOO; break;
            case 'k': castling_rights_ |= BLACK_OO; break;
            case 'q': castling_rights_ |= BLACK_OOO; break;
            default: return false;
        }
    }

    return true;
}

/*!
 * @note Use another ascii trick with file_char and rank_char
 *
 * Example for file:
 * file_char = 'e';
 * 'e' - 'a' as chars
 * 101 - 97 as integers
 * 4 is 'e' file (bcs numeration starts with 0)
 *
 * Example for rank:
 * rank_char = '3';
 * '3' - '1' as chars
 * 51 - 49 as integers
 * 2 is a 3-d rank (bcs numeration starts with 0)
 */
bool Position::parse_en_passant(const std::string& ep) {
    if (ep == "-") {
        en_passant_square_ = std::nullopt;
        return true;
    }

    // En passant format column and raw (like "e3")
    if (ep.length() != 2) {
        return false;
    }

    char file_char = ep[0];
    char rank_char = ep[1];

    // board limits
    if (file_char < 'a' || file_char > 'h' || rank_char < '1' || rank_char > '8') {
        return false;
    }

    // ascii trick
    int file = file_char - 'a';
    int rank = rank_char - '1';

    en_passant_square_ = static_cast<Square>(rank * 8 + file);
    return true;
}

std::string Position::to_fen() const {
    std::string fen;

    // 1. Pieces positions
    for (int rank = 7; rank >= 0; rank--) {
        int empty_count = 0;

        for (int file = 0; file < 8; file++) {
            Square sq = static_cast<Square>(rank * 8 + file);
            Piece piece = bitboards_.piece_on(sq);

            if (piece == PIECE_NUM) {
                empty_count++;
            } else {
                if (empty_count > 0) {
                    fen += std::to_string(empty_count);
                    empty_count = 0;
                }
                fen += pieces_notations[piece];
            }
        }

        if (empty_count > 0) {
            fen += std::to_string(empty_count);
        }

        if (rank > 0) {
            fen += '/';
        }
    }

    // 2. Move order
    fen += ' ';
    fen += (side_to_move_ == WHITE) ? 'w' : 'b';

    // 3. Castling rights
    fen += ' ';
    if (castling_rights_ == NO_CASTLING) {
        fen += '-';
    } else {
        if (castling_rights_ & WHITE_OO) fen += 'K';
        if (castling_rights_ & WHITE_OOO) fen += 'Q';
        if (castling_rights_ & BLACK_OO) fen += 'k';
        if (castling_rights_ & BLACK_OOO) fen += 'q';
    }

    // 4. En passant
    fen += ' ';
    if (en_passant_square_.has_value()) {
        Square sq = en_passant_square_.value();
        int file = sq % 8;
        int rank = sq / 8;
        fen += static_cast<char>('a' + file);
        fen += static_cast<char>('1' + rank);
    } else {
        fen += '-';
    }

    // 5 і 6. Counters
    fen += ' ' + std::to_string(halfmove_clock_);
    fen += ' ' + std::to_string(fullmove_number_);

    return fen;
}

void Position::print() const {
    std::cout << "\n  +---+---+---+---+---+---+---+---+\n";

    for (int rank = 7; rank >= 0; rank--) {
        std::cout << (rank + 1) << " |";

        for (int file = 0; file < 8; file++) {
            Square sq = static_cast<Square>(rank * 8 + file);
            Piece piece = bitboards_.piece_on(sq);
            std::cout << " " << pieces_notations[piece] << " |";
        }

        std::cout << "\n  +---+---+---+---+---+---+---+---+\n";
    }

    std::cout << "    a   b   c   d   e   f   g   h\n\n";
    std::cout << "FEN: " << to_fen() << "\n";
    std::cout << "Side to move: " << (side_to_move_ == WHITE ? "White" : "Black") << "\n";
}

bool Position::has_king(Color c) const {
    Bitboard kings = bitboards_.pieces(KING, c);
    return kings != 0;
}

} // namespace Rmagician
