#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Evaluation.h"

namespace Rmagician {
namespace {

using ::testing::Eq;

int evaluate_fen(const std::string& fen) {
    Position position;
    EXPECT_TRUE(position.set_from_fen(fen));
    return Evaluation::evaluate(position);
}

TEST(EvaluationTest, StartPositionIsEqual) {
    Position position;
    position.set_start_position();

    EXPECT_THAT(Evaluation::evaluate(position), Eq(0));
}

TEST(EvaluationTest, EvaluationDoesNotDependOnSideToMove) {
    const std::string white_to_move = "4k3/8/8/8/3N4/8/8/4K3 w - - 0 1";
    const std::string black_to_move = "4k3/8/8/8/3N4/8/8/4K3 b - - 0 1";

    EXPECT_THAT(evaluate_fen(white_to_move), Eq(evaluate_fen(black_to_move)));
}

TEST(EvaluationTest, ExtraQueenProducesLargeMaterialAdvantage) {
    const int white_extra_queen = evaluate_fen("4k3/8/8/8/8/8/8/3QK3 w - - 0 1");
    const int black_extra_queen = evaluate_fen("3qk3/8/8/8/8/8/8/4K3 w - - 0 1");

    EXPECT_GT(white_extra_queen, 800);
    EXPECT_LT(black_extra_queen, -800);
    EXPECT_THAT(white_extra_queen, Eq(-black_extra_queen));
}

TEST(EvaluationTest, CentralizedKnightScoresBetterThanCornerKnight) {
    const int center_knight = evaluate_fen("4k3/8/8/8/3N4/8/8/4K3 w - - 0 1");
    const int corner_knight = evaluate_fen("4k3/8/8/8/8/8/8/N3K3 w - - 0 1");

    EXPECT_GT(center_knight, corner_knight);
}

TEST(EvaluationTest, ConnectedPawnsScoreBetterThanDoubledIsolatedPawns) {
    const int connected = evaluate_fen("4k3/8/8/8/8/3P4/2P5/4K3 w - - 0 1");
    const int doubled_isolated = evaluate_fen("4k3/8/8/8/8/2P5/2P5/4K3 w - - 0 1");

    EXPECT_GT(connected, doubled_isolated);
}

TEST(EvaluationTest, ConnectedPawnChainBeatsSeparatedIsolatedPawns) {
    const int connected = evaluate_fen("4k3/8/8/8/2PP4/8/8/4K3 w - - 0 1");
    const int separated = evaluate_fen("4k3/8/8/8/2P2P2/8/8/4K3 w - - 0 1");

    EXPECT_GT(connected, separated);
}

TEST(EvaluationTest, AdvancedPassedPawnScoresBetterThanStartingPassedPawn) {
    const int pawn_on_e2 = evaluate_fen("4k3/8/8/8/8/8/4P3/4K3 w - - 0 1");
    const int pawn_on_e5 = evaluate_fen("4k3/8/8/4P3/8/8/8/4K3 w - - 0 1");

    EXPECT_GT(pawn_on_e5, pawn_on_e2);
}

TEST(EvaluationTest, SymmetricOpenGameRemainsEqual) {
    const std::string fen = "rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 2";

    EXPECT_THAT(evaluate_fen(fen), Eq(0));
}

TEST(EvaluationTest, SinglePawnColorMirrorFlipsScoreSign) {
    const int white_pawn = evaluate_fen("4k3/8/8/8/2P5/8/8/4K3 w - - 0 1");
    const int black_pawn = evaluate_fen("4k3/8/8/2p5/8/8/8/4K3 w - - 0 1");

    EXPECT_THAT(white_pawn, Eq(-black_pawn));
}

} // namespace
} // namespace Rmagician
