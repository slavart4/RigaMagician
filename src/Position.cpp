#include "Position.h"
#include "utils/FenHelper.h"

namespace Rmagician {

void Position::set_start_position() {
    // Standard start position in FEN notation
    (void)set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

bool Position::set_from_fen(const std::string& fen) {
    return FenHelper::parse_fen(fen, bitboards_, pd_);
}

std::string Position::to_fen() const {
    return FenHelper::to_fen(bitboards_, pd_);
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
    std::cout << "Side to move: " << (pd_.side_to_move == WHITE ? "White" : "Black") << "\n";
}

void Position::make_move(Move& m, UndoInfo& undo) {
    Piece moving_piece = bitboards_.piece_on(m.from());

    // Save data for further unmake_move
    undo.castling_rights = pd_.castling_rights;
    undo.en_passant_square = pd_.en_passant_square;
    undo.halfmove_clock = pd_.halfmove_clock;
    undo.captured_piece = bitboards_.piece_on(m.to());

    pd_.en_passant_square = std::nullopt;
    pd_.halfmove_clock++;

    if (m.flag() == CAPTURE || undo.captured_piece != PIECE_NUM) {
        bitboards_.remove_piece(m.to());
        pd_.halfmove_clock = 0;
    }

    bitboards_.move_piece(m.from(), m.to());

    if ((moving_piece == W_PAWN || moving_piece == B_PAWN)) {
        pawn_move(m, m.flag());
    }

    // Update castling rights if king moving
    if (moving_piece == W_KING) pd_.castling_rights &= ~(WHITE_OO | WHITE_OOO);
    if (moving_piece == B_KING) pd_.castling_rights &= ~(BLACK_OO | BLACK_OOO);

    // Update castling rights if rook moving
    if (m.from() == H1 || m.to() == H1) pd_.castling_rights &= ~WHITE_OO;
    if (m.from() == A1 || m.to() == A1) pd_.castling_rights &= ~WHITE_OOO;
    if (m.from() == H8 || m.to() == H8) pd_.castling_rights &= ~BLACK_OO;
    if (m.from() == A8 || m.to() == A8) pd_.castling_rights &= ~BLACK_OOO;

    // Castling
    if (m.flag() == KING_CASTLE) {
        if (pd_.side_to_move == WHITE) bitboards_.move_piece(H1, F1);
        else bitboards_.move_piece(H8, F8);
    } else if (m.flag() == QUEEN_CASTLE) {
        if (pd_.side_to_move == WHITE) bitboards_.move_piece(A1, D1);
        else bitboards_.move_piece(A8, D8);
    }

    // Change side to move
    if (pd_.side_to_move == BLACK) pd_.fullmove_number++;
    pd_.side_to_move = pd_.side_to_move == WHITE ? BLACK : WHITE;
}

void Position::pawn_move(Move &m, MoveFlag flags) {
    pd_.halfmove_clock = 0; // Pawns moves reset counter by rule

    // Double Pawn Push - set en passant square
    if (flags == DOUBLE_PAWN_PUSH) {
        pd_.en_passant_square = (pd_.side_to_move == WHITE) ?
            static_cast<Square>(m.to() + SOUTH) :
            static_cast<Square>(m.to() + NORTH);
    }

    // If there was en passant capture - square is no longer en passant
    if (flags == EN_PASSANT) {
        pd_.en_passant_square = std::nullopt;
    }

    if (flags == PROMOTION) {
        bitboards_.move_piece(m.from(), m.to());
    }
}

void Position::undo_move(Move& m, UndoInfo& undo) {
    pd_.side_to_move = (pd_.side_to_move == WHITE) ? BLACK : WHITE;
    if (pd_.side_to_move == BLACK) pd_.fullmove_number--;

    Square from = m.from();
    Square to = m.to();
    MoveFlag flags = m.flag();

    bitboards_.move_piece(to, from);

    if (undo.captured_piece != PIECE_NUM) {
        if (flags == EN_PASSANT) {
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

    if (flags == KING_CASTLE) {
        if (pd_.side_to_move == WHITE) bitboards_.move_piece(F1, H1);
        else bitboards_.move_piece(F8, H8);
    } else if (flags == QUEEN_CASTLE) {
        if (pd_.side_to_move == WHITE) bitboards_.move_piece(D1, A1);
        else bitboards_.move_piece(D8, A8);
    }

    if (flags == PROMOTION) {
        Piece pawn = (pd_.side_to_move == WHITE) ? W_PAWN : B_PAWN;
        bitboards_.remove_piece(from);
        bitboards_.set_piece(from, pawn);
    }

    pd_.castling_rights = undo.castling_rights;
    pd_.en_passant_square = undo.en_passant_square;
    pd_.halfmove_clock = undo.halfmove_clock;
}

bool Position::has_king(Color c) const {
    Bitboard kings = bitboards_.pieces(KING, c);
    return kings != 0;
}

} // namespace Rmagician
