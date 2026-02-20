#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Move.h"

namespace Rmagician {
namespace {

using ::testing::Eq;

TEST(MoveTest, EncodesAndDecodesFieldsCorrectly) {
    const Move move(B2, E4, W_QUEEN, CAPTURE);

    EXPECT_THAT(move.from(), Eq(B2));
    EXPECT_THAT(move.to(), Eq(E4));
    EXPECT_THAT(move.promotion_piece(), Eq(W_QUEEN));
    EXPECT_THAT(move.flag(), Eq(CAPTURE));
}

TEST(MoveTest, UsesQuietFlagByDefault) {
    const Move move(G1, F3, PIECE_NUM);

    EXPECT_THAT(move.from(), Eq(G1));
    EXPECT_THAT(move.to(), Eq(F3));
    EXPECT_THAT(move.promotion_piece(), Eq(PIECE_NUM));
    EXPECT_THAT(move.flag(), Eq(QUIET));
}

TEST(MoveTest, StoresPromotionPieceAndPromotionFlag) {
    const Move move(E7, E8, W_QUEEN, PROMOTION);

    EXPECT_THAT(move.from(), Eq(E7));
    EXPECT_THAT(move.to(), Eq(E8));
    EXPECT_THAT(move.promotion_piece(), Eq(W_QUEEN));
    EXPECT_THAT(move.flag(), Eq(PROMOTION));
}

TEST(MoveTest, IsNoneTrueForZeroEncodedMove) {
    const Move move(A1, A1, W_PAWN, QUIET);
    EXPECT_TRUE(move.is_none());
}

TEST(MoveTest, IsNoneFalseForRegularMove) {
    const Move move(A2, A3, W_PAWN, QUIET);
    EXPECT_FALSE(move.is_none());
}

TEST(MoveTest, MasksPromotionPieceAndFlagToFourBits) {
    const auto out_of_range_piece = static_cast<Piece>(31);     // 0b1_1111
    const auto out_of_range_flag = static_cast<MoveFlag>(31);   // 0b1_1111
    const Move move(H7, H8, out_of_range_piece, out_of_range_flag);

    EXPECT_THAT(move.promotion_piece(), Eq(static_cast<Piece>(15)));
    EXPECT_THAT(move.flag(), Eq(static_cast<MoveFlag>(15)));
    EXPECT_THAT(move.from(), Eq(H7));
    EXPECT_THAT(move.to(), Eq(H8));
}

} // namespace
} // namespace Rmagician
