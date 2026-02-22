#ifndef RIGAMAGICIAN_UCI_H
#define RIGAMAGICIAN_UCI_H

#include <string>

#include "Position.h"
#include "Search.h"
#include "MoveGenerator.h"

namespace Rmagician {

class UCI {
public:
    static void loop();
private:
    static bool parse_position_command(const std::string& line, Position& pos);
    static bool is_legal_move(Position& pos, Move m);
    static bool parse_uci_move(Position& pos, const std::string& move_text, Move& out_move);
};

} // Rmagician

#endif //RIGAMAGICIAN_UCI_H
