#include "FenUtils.h"
#include <sstream>
#include <vector>

namespace Rmagician {

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
bool FenUtils::parse_fen(const std::string& fen, Bitboards& bb, PositionDetails& pd) {
    bb = Bitboards();
    pd = PositionDetails{};

    std::istringstream iss(fen);
    std::vector<std::string> parts;
    std::string part;

    while (iss >> part) {
        parts.push_back(part);
    }

    if (parts.size() < 4) {
        return false;
    }

    if (!parse_piece_placement(parts[0], bb)) {
        return false;
    }

    if (parts[1] == "w") {
        pd.side_to_move = WHITE;
    } else if (parts[1] == "b") {
        pd.side_to_move = BLACK;
    } else {
        return false;
    }

    if (!parse_castling_rights(parts[2], pd)) {
        return false;
    }

    if (!parse_en_passant(parts[3], pd)) {
        return false;
    }

    if (parts.size() > 4) {
        try {
            pd.halfmove_clock = std::stoi(parts[4]);
        } catch (...) {
            pd.halfmove_clock = 0;
        }
    } else {
        pd.halfmove_clock = 0;
    }

    if (parts.size() > 5) {
        try {
            pd.fullmove_number = std::stoi(parts[5]);
        } catch (...) {
            pd.fullmove_number = 1;
        }
    } else {
        pd.fullmove_number = 1;
    }

    return true;
}

bool FenUtils::parse_piece_placement(const std::string& placement, Bitboards& bb) {
    int rank = 7;
    int file = 0;

    for (char c : placement) {
        if (c == '/') {
            rank--;
            file = 0;
            if (rank < 0) {
                return false;
            }
        } else if (std::isdigit(c)) {
            int empty_squares = c - '0';
            file += empty_squares;
            if (file > 8) {
                return false;
            }
        } else {
            if (file >= 8) {
                return false;
            }

            Square sq = static_cast<Square>(rank * 8 + file);
            if (!notations_pieces.contains(c)) {
                return false;
            }

            Piece piece = notations_pieces[c];
            bb.set_piece(sq, piece);
            file++;
        }
    }

    return rank == 0 && file == 8;
}

bool FenUtils::parse_castling_rights(const std::string& castling, PositionDetails& pd) {
    pd.castling_rights = NO_CASTLING;

    if (castling == "-") {
        return true;
    }

    for (char c : castling) {
        switch (c) {
            case 'K': pd.castling_rights |= WHITE_OO; break;
            case 'Q': pd.castling_rights |= WHITE_OOO; break;
            case 'k': pd.castling_rights |= BLACK_OO; break;
            case 'q': pd.castling_rights |= BLACK_OOO; break;
            default: return false;
        }
    }

    return true;
}

bool FenUtils::parse_en_passant(const std::string& ep, PositionDetails& pd) {
    if (ep == "-") {
        pd.en_passant_square = std::nullopt;
        return true;
    }

    if (ep.length() != 2) {
        return false;
    }

    const char file_char = ep[0];
    const char rank_char = ep[1];

    if (file_char < 'a' || file_char > 'h' || rank_char < '1' || rank_char > '8') {
        return false;
    }

    const int file = file_char - 'a';
    const int rank = rank_char - '1';
    pd.en_passant_square = static_cast<Square>(rank * 8 + file);
    return true;
}

std::string FenUtils::to_fen(const Bitboards& bb, const PositionDetails& pd) {
    std::string fen;

    for (int rank = 7; rank >= 0; rank--) {
        int empty_count = 0;

        for (int file = 0; file < 8; file++) {
            const Square sq = static_cast<Square>(rank * 8 + file);
            const Piece piece = bb.piece_on(sq);

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

    fen += ' ';
    fen += (pd.side_to_move == WHITE) ? 'w' : 'b';

    fen += ' ';
    if (pd.castling_rights == NO_CASTLING) {
        fen += '-';
    } else {
        if (pd.castling_rights & WHITE_OO) fen += 'K';
        if (pd.castling_rights & WHITE_OOO) fen += 'Q';
        if (pd.castling_rights & BLACK_OO) fen += 'k';
        if (pd.castling_rights & BLACK_OOO) fen += 'q';
    }

    fen += ' ';
    if (pd.en_passant_square.has_value()) {
        const Square sq = pd.en_passant_square.value();
        fen += static_cast<char>('a' + (sq % 8));
        fen += static_cast<char>('1' + (sq / 8));
    } else {
        fen += '-';
    }

    fen += ' ' + std::to_string(pd.halfmove_clock);
    fen += ' ' + std::to_string(pd.fullmove_number);
    return fen;
}

} // namespace Rmagician
