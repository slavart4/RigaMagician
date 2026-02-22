#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "MoveGenerator.h"

namespace Rmagician {
namespace {

using ::testing::Eq;

bool has_move(const MoveList& list, Square from, Square to, MoveFlag flag) {
    for (int i = 0; i < list.count; ++i) {
        const Move& m = list.moves[i];
        if (m.from() == from && m.to() == to && m.flag() == flag) {
            return true;
        }
    }
    return false;
}

bool has_promotion(const MoveList& list, Square from, Square to, Piece promo_piece) {
    for (int i = 0; i < list.count; ++i) {
        const Move& m = list.moves[i];
        if (m.from() == from && m.to() == to && m.flag() == PROMOTION && m.promotion_piece() == promo_piece) {
            return true;
        }
    }
    return false;
}

TEST(MoveGeneratorTest, StartPositionHasTwentyPseudoLegalMoves) {
    Position position;
    MoveGenerator generator;
    MoveList list;

    position.set_start_position();
    generator.generate_moves(position, list);

    EXPECT_THAT(list.count, Eq(20));
}

TEST(MoveGeneratorTest, StartPositionContainsTypicalPawnAndKnightMoves) {
    Position position;
    MoveGenerator generator;
    MoveList list;

    position.set_start_position();
    generator.generate_moves(position, list);

    EXPECT_TRUE(has_move(list, E2, E4, DOUBLE_PAWN_PUSH));
    EXPECT_TRUE(has_move(list, G1, F3, QUIET));
    EXPECT_TRUE(has_move(list, B1, C3, QUIET));
}

TEST(MoveGeneratorTest, GeneratesCastlingWhenPathIsClearAndSafe) {
    Position position;
    MoveGenerator generator;
    MoveList list;

    ASSERT_TRUE(position.set_from_fen("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1"));
    generator.generate_moves(position, list);

    EXPECT_TRUE(has_move(list, E1, G1, KING_CASTLE));
    EXPECT_TRUE(has_move(list, E1, C1, QUEEN_CASTLE));
}

TEST(MoveGeneratorTest, DoesNotGenerateKingSideCastlingThroughCheck) {
    Position position;
    MoveGenerator generator;
    MoveList list;

    ASSERT_TRUE(position.set_from_fen("4kr2/8/8/8/8/8/8/R3K2R w KQ - 0 1"));
    generator.generate_moves(position, list);

    EXPECT_FALSE(has_move(list, E1, G1, KING_CASTLE));
    EXPECT_TRUE(has_move(list, E1, C1, QUEEN_CASTLE));
}

TEST(MoveGeneratorTest, GeneratesEnPassantWhenAvailable) {
    Position position;
    MoveGenerator generator;
    MoveList list;

    ASSERT_TRUE(position.set_from_fen("4k3/8/8/3pP3/8/8/8/4K3 w - d6 0 1"));
    generator.generate_moves(position, list);

    EXPECT_TRUE(has_move(list, E5, D6, EN_PASSANT));
}

TEST(MoveGeneratorTest, BlackPromotionUsesBlackPromotionPieces) {
    Position position;
    MoveGenerator generator;
    MoveList list;

    ASSERT_TRUE(position.set_from_fen("k7/8/8/8/8/8/4p3/7K b - - 0 1"));
    generator.generate_moves(position, list);

    EXPECT_TRUE(has_promotion(list, E2, E1, B_QUEEN));
    EXPECT_TRUE(has_promotion(list, E2, E1, B_ROOK));
    EXPECT_TRUE(has_promotion(list, E2, E1, B_BISHOP));
    EXPECT_TRUE(has_promotion(list, E2, E1, B_KNIGHT));
}

} // namespace
} // namespace Rmagician
