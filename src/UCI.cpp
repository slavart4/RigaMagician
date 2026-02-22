#include <cctype>
#include <iostream>
#include <sstream>
#include <vector>

#include "UCI.h"
#include "utils/BitboardUtils.h"

namespace Rmagician {

void UCI::loop() {
    std::string line, token;
    Position pos;
    pos.set_start_position();
    Search search;

    while (std::getline(std::cin, line)) {
        std::istringstream is(line);
        is >> token;

        if (token == "uci") {
            std::cout << "id name Rmagician 1.0" << std::endl;
            std::cout << "id author YourName" << std::endl;
            std::cout << "uciok" << std::endl;
        }
        else if (token == "isready") {
            std::cout << "readyok" << std::endl;
        }
        else if (token == "ucinewgame") {
            pos.set_start_position();
        }
        else if (token == "position") {
            (void)parse_position_command(line, pos);
        }
        else if (token == "go") {
            Search::SearchParams params {4};
            std::string go_token;
            while (is >> go_token) {
                if (go_token == "depth") {
                    int depth = 0;
                    if (is >> depth && depth > 0) {
                        params.depth = depth;
                    }
                }
            }

            Move best_move = search.find_best_move(pos, params);
            std::cout << "bestmove " << BitboardUtils::move_to_string(best_move) << std::endl;
        }
        else if (token == "stop") {
            // Synchronous search currently: stop is accepted but no-op.
        }
        else if (token == "quit") {
            break;
        }
    }
}

bool UCI::parse_position_command(const std::string& line, Position& pos) {
    std::istringstream is(line);
    std::string token;
    is >> token; // position

    if (!(is >> token)) {
        return false;
    }

    if (token == "startpos") {
        pos.set_start_position();
    } else if (token == "fen") {
        std::vector<std::string> fen_parts;
        std::string part;
        for (int i = 0; i < 6 && (is >> part); i++) {
            fen_parts.push_back(part);
        }

        if (fen_parts.size() != 6) {
            return false;
        }

        std::string fen = fen_parts[0];
        for (size_t i = 1; i < fen_parts.size(); i++) {
            fen += " " + fen_parts[i];
        }

        if (!pos.set_from_fen(fen)) {
            pos.set_start_position();
            return false;
        }
    } else {
        return false;
    }

    std::string maybe_moves;
    if (!(is >> maybe_moves) || maybe_moves != "moves") {
        return true;
    }

    std::string move_text;
    while (is >> move_text) {
        Move move;
        if (!parse_uci_move(pos, move_text, move)) {
            return false;
        }
        UndoInfo undo;
        pos.make_move(move, undo);
    }

    return true;
}

bool UCI::is_legal_move(Position& pos, Move m) {
    UndoInfo undo;
    pos.make_move(m, undo);
    const Color us = pos.side_to_move();
    const Color them = (us == WHITE) ? BLACK : WHITE;
    const bool legal = !pos.is_square_attacked(pos.king_square(them), us);
    pos.undo_move(m, undo);
    return legal;
}

bool UCI::parse_uci_move(Position& pos, const std::string& move_text, Move& out_move) {
    if (move_text.size() < 4) {
        return false;
    }

    const Square from = BitboardUtils::string_to_square(move_text.substr(0, 2));
    const Square to = BitboardUtils::string_to_square(move_text.substr(2, 2));
    if (from == SQUARE_NUM || to == SQUARE_NUM) {
        return false;
    }

    MoveGenerator generator;
    MoveList list;
    generator.generate_moves(pos, list);

    const bool has_promotion_suffix = move_text.size() == 5;
    Piece promotion_piece = PIECE_NUM;
    if (has_promotion_suffix) {
        char promotion_char = static_cast<char>(std::tolower(move_text[4]));
        if (promotion_char != 'q' && promotion_char != 'r' &&
            promotion_char != 'b' && promotion_char != 'n') {
            return false;
        }

        if (pos.side_to_move() == WHITE) {
            promotion_char = static_cast<char>(std::toupper(promotion_char));
        }

        auto it = notations_pieces.find(promotion_char);
        if (it == notations_pieces.end()) {
            return false;
        }
        promotion_piece = it->second;
    }

    for (int i = 0; i < list.count; i++) {
        Move candidate = list.moves[i];
        if (candidate.from() != from || candidate.to() != to) {
            continue;
        }

        if (candidate.flag() == PROMOTION) {
            if (!has_promotion_suffix) {
                continue;
            }
            if (candidate.promotion_piece() != promotion_piece) {
                continue;
            }
        } else if (has_promotion_suffix) {
            continue;
        }

        if (!is_legal_move(pos, candidate)) {
            continue;
        }

        out_move = candidate;
        return true;
    }

    return false;
}
} // Rmagician
