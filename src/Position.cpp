#include "Position.h"
#include "utils/FenUtils.h"
#include "utils/BitboardUtils.h"

namespace Rmagician {

void Position::set_start_position() {
    // Standard start position in FEN notation
    (void)set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

bool Position::set_from_fen(const std::string& fen) {
    return FenUtils::parse_fen(fen, bitboards_, pd_);
}

std::string Position::to_fen() const {
    return FenUtils::to_fen(bitboards_, pd_);
}

void Position::print() const {
    std::cout << "\n  +-----+-----+-----+-----+-----+-----+-----+-----+\n";

    for (int rank = 7; rank >= 0; rank--) {
        std::cout << (rank + 1) << " |";

        for (int file = 0; file < 8; file++) {
            Square sq = static_cast<Square>(rank * 8 + file);
            Piece piece = bitboards_.piece_on(sq);
            std::cout << "  " << pieces_notations[piece] << "  |";
        }

        std::cout << "\n  +-----+-----+-----+-----+-----+-----+-----+-----+\n";
    }

    std::cout << "    a   b   c   d   e   f   g   h\n\n";
    std::cout << "FEN: " << to_fen() << "\n";
    std::cout << "Side to move: " << (pd_.side_to_move == WHITE ? "White" : "Black") << "\n";
}

void Position::make_move(Move& m, UndoInfo& undo) {
    Piece moving_piece = bitboards_.piece_on(m.from());

    // Save data for further unmake_move
    undo.castling_rights =      pd_.castling_rights;
    undo.en_passant_square =    pd_.en_passant_square;
    undo.halfmove_clock =       pd_.halfmove_clock;
    undo.captured_piece =       bitboards_.piece_on(m.to());

    pd_.en_passant_square = std::nullopt;
    pd_.halfmove_clock++;

    bitboards_.move_piece(m.from(), m.to());

    if (m.flag() == CAPTURE || undo.captured_piece != PIECE_NUM) {
        pd_.halfmove_clock = 0;
    }

    if (moving_piece == W_PAWN || moving_piece == B_PAWN) {
        process_pawn_move(m);
    }

    if (moving_piece == W_KING || moving_piece == B_KING
        || moving_piece == W_ROOK || moving_piece == B_ROOK) {
        process_castling_rights(m);
    }

    // Castling
    process_castling(m);

    // Change side to move
    if (pd_.side_to_move == BLACK) pd_.fullmove_number++;
    pd_.side_to_move = pd_.side_to_move == WHITE ? BLACK : WHITE;
}

void Position::process_pawn_move(Move &m) {
    pd_.halfmove_clock = 0; // Pawns moves reset counter by rule

    // Double Pawn Push - set en passant square
    if (m.flag() == DOUBLE_PAWN_PUSH) {
        pd_.en_passant_square = (pd_.side_to_move == WHITE) ?
            static_cast<Square>(m.to() + SOUTH) :
            static_cast<Square>(m.to() + NORTH);
    }

    // If there was en passant capture - square is no longer en passant
    if (m.flag() == EN_PASSANT) {
        pd_.en_passant_square = std::nullopt;
    }

    if (m.flag() == PROMOTION) {
        const bool valid_promotion_rank =
            (pd_.side_to_move == WHITE && BitboardUtils::rank_of(m.to()) == 7) ||
            (pd_.side_to_move == BLACK && BitboardUtils::rank_of(m.to()) == 0);

        if (!valid_promotion_rank) {
            return;
        }

        Piece promoted_piece = m.promotion_piece();

        // Basic safety: promotion must be to a real non-king piece.
        if (promoted_piece == PIECE_NUM || promoted_piece == W_KING || promoted_piece == B_KING) {
            return;
        }

        // Optional color safety check.
        if (pd_.side_to_move == WHITE && promoted_piece >= B_PAWN) {
            return;
        }
        if (pd_.side_to_move == BLACK && promoted_piece <= W_KING) {
            return;
        }

        bitboards_.remove_piece(m.to()); // remove pawn that just moved
        bitboards_.set_piece(m.to(), promoted_piece);
    }
}

void Position::process_castling_rights(Move &m) {
    Piece moving_piece = bitboards_.piece_on(m.from());

    // Update castling rights if king moving
    if (moving_piece == W_KING) pd_.castling_rights &= ~(WHITE_OO | WHITE_OOO);
    if (moving_piece == B_KING) pd_.castling_rights &= ~(BLACK_OO | BLACK_OOO);

    // Update castling rights if rook moving from start position
    if (moving_piece == W_ROOK || moving_piece == B_ROOK) {
        if (m.from() == H1 || m.to() == H1) pd_.castling_rights &= ~WHITE_OO;
        if (m.from() == A1 || m.to() == A1) pd_.castling_rights &= ~WHITE_OOO;
        if (m.from() == H8 || m.to() == H8) pd_.castling_rights &= ~BLACK_OO;
        if (m.from() == A8 || m.to() == A8) pd_.castling_rights &= ~BLACK_OOO;
    }
}

void Position::process_castling(Move &m) {
    if (m.flag() == KING_CASTLE) {
        if (pd_.side_to_move == WHITE) bitboards_.move_piece(H1, F1);
        else bitboards_.move_piece(H8, F8);
    } else if (m.flag() == QUEEN_CASTLE) {
        if (pd_.side_to_move == WHITE) bitboards_.move_piece(A1, D1);
        else bitboards_.move_piece(A8, D8);
    }
}

void Position::undo_move(Move& m, UndoInfo& undo) {
    const Square from = m.from();
    const Square to = m.to();
    const MoveFlag flag = m.flag();

    pd_.side_to_move = (pd_.side_to_move == WHITE) ? BLACK : WHITE;
    if (pd_.side_to_move == BLACK) pd_.fullmove_number--;

    bitboards_.move_piece(to, from);

    if (flag == KING_CASTLE) {
        if (pd_.side_to_move == WHITE) bitboards_.move_piece(F1, H1);
        else bitboards_.move_piece(F8, H8);
    } else if (flag == QUEEN_CASTLE) {
        if (pd_.side_to_move == WHITE) bitboards_.move_piece(D1, A1);
        else bitboards_.move_piece(D8, A8);
    }

    if (flag == PROMOTION) {
        const Piece promoted_piece = m.promotion_piece();
        if (promoted_piece != PIECE_NUM) {
            const Piece pawn = (pd_.side_to_move == WHITE) ? W_PAWN : B_PAWN;
            bitboards_.remove_piece(from);
            bitboards_.set_piece(from, pawn);
        }
    }

    if (undo.captured_piece != PIECE_NUM) {
        if (flag == EN_PASSANT) {
            Square victim_sq {};
            if(pd_.side_to_move == WHITE) {
                victim_sq = static_cast<Square>(to + SOUTH);
            } else {
                victim_sq = static_cast<Square>(to + NORTH);
            }
            bitboards_.set_piece(victim_sq, undo.captured_piece);
        } else {
            bitboards_.set_piece(to, undo.captured_piece);
        }
    }

    pd_.castling_rights = undo.castling_rights;
    pd_.en_passant_square = undo.en_passant_square;
    pd_.halfmove_clock = undo.halfmove_clock;
}

bool Position::has_king(Color c) const {
    Bitboard kings = bitboards_.pieces(KING, c);
    return kings != 0;
}

/// TODO: fix
/*!
 * @note
 * Bitboards are checked with bitwise AND:
 * `attacks_mask & pieces_mask`.
 * If result is non-zero, at least one attacker exists on an attacked square.
 *
 * Example (8-bit fragment for simplicity):
 * attacks_mask = 0b00101000
 * pieces_mask  = 0b00001000
 * AND result   = 0b00001000 (non-zero) -> square is attacked.
 *
 * If pieces_mask = 0b00010000 then:
 * 0b00101000 & 0b00010000 = 0b00000000 -> no attacker on attack squares.
 */
bool Position::is_square_attacked(Square sq, Color attacker_color) const {
    Bitboard pawns = bitboards_.pieces(PAWN, attacker_color);
    if (attacks_.get_pawn_attacks(sq, (attacker_color == WHITE ? BLACK : WHITE)) & pawns)
        return true;

    Bitboard knights = bitboards_.pieces(KNIGHT, attacker_color);
    if (attacks_.get_knight_attacks(sq) & knights)
        return true;

    Bitboard king = bitboards_.pieces(KING, attacker_color);
    if (attacks_.get_king_attacks(sq) & king)
        return true;

    Bitboard occupied = bitboards_.all();

    Bitboard bishops_queens = bitboards_.pieces(BISHOP, attacker_color) |
                              bitboards_.pieces(QUEEN, attacker_color);
    if (attacks_.get_bishop_attacks(sq, occupied) & bishops_queens)
        return true;

    Bitboard rooks_queens = bitboards_.pieces(ROOK, attacker_color) |
                            bitboards_.pieces(QUEEN, attacker_color);
    if (attacks_.get_rook_attacks(sq, occupied) & rooks_queens)
        return true;

    return false;
}

} // namespace Rmagician
