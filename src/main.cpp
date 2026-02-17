#include "Position.h"

/// TODO: add tests using gtest gmock
///  tests for fen parse/convert
///  tests for all board utils

using namespace Rmagician;

int main() {
    Position position;
    position.set_start_position();
    position.print();

    return 0;
}