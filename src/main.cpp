#include "Position.h"

/// TODO: add tests using gtest gmock
///  test for fen parse/convert

using namespace Rmagician;

int main() {
    Position position;
    position.set_start_position();
    position.print();

    return 0;
}