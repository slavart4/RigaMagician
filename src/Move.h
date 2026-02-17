#ifndef RIGAMAGICIAN_MOVE_H
#define RIGAMAGICIAN_MOVE_H

#include "Types.h"

namespace Rmagician {

struct Move {
    Move() = default;
    Move(Square f, Square t, MoveFlag fl = QUIET, Piece promo = PIECE_NUM)
        : from(f), to(t), flag(fl), promotion(promo) {}

    Square from;
    Square to;
    MoveFlag flag;
    Piece promotion;
};

struct UndoInfo {
    Piece captured = PIECE_NUM;
    Square ep_square {};
    uint8_t castling_rights {};
    int halfmove_clock {};
};

struct State {
    Move move;
    UndoInfo undo;
};

}

#endif //RIGAMAGICIAN_MOVE_H