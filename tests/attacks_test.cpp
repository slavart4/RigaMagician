#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Attacks.h"

namespace Rmagician {
namespace {

using ::testing::Eq;

class AttacksTest : public ::testing::Test {
protected:
    void SetUp() override {
        attacks_.init();
    }

    Attacks attacks_;
};

TEST_F(AttacksTest, PawnAttacksFollowColorAndBoardEdges) {
    EXPECT_THAT(attacks_.get_pawn_attacks(E4, WHITE), Eq((1ULL << D5) | (1ULL << F5)));
    EXPECT_THAT(attacks_.get_pawn_attacks(E4, BLACK), Eq((1ULL << D3) | (1ULL << F3)));

    EXPECT_THAT(attacks_.get_pawn_attacks(A2, WHITE), Eq(1ULL << B3));
    EXPECT_THAT(attacks_.get_pawn_attacks(H7, BLACK), Eq(1ULL << G6));
}

TEST_F(AttacksTest, KnightAttacksArePrecomputedCorrectlyForCenterAndCorner) {
    const Bitboard d4_attacks =
        (1ULL << B3) | (1ULL << B5) | (1ULL << C2) | (1ULL << C6) |
        (1ULL << E2) | (1ULL << E6) | (1ULL << F3) | (1ULL << F5);

    EXPECT_THAT(attacks_.get_knight_attacks(D4), Eq(d4_attacks));
    EXPECT_THAT(attacks_.get_knight_attacks(A1), Eq((1ULL << B3) | (1ULL << C2)));
}

TEST_F(AttacksTest, KingAttacksArePrecomputedCorrectlyForCenterAndCorner) {
    const Bitboard d4_attacks =
        (1ULL << C3) | (1ULL << C4) | (1ULL << C5) |
        (1ULL << D3) | (1ULL << D5) |
        (1ULL << E3) | (1ULL << E4) | (1ULL << E5);

    EXPECT_THAT(attacks_.get_king_attacks(D4), Eq(d4_attacks));
    EXPECT_THAT(attacks_.get_king_attacks(A1), Eq((1ULL << A2) | (1ULL << B1) | (1ULL << B2)));
}

TEST_F(AttacksTest, RookAttacksCoverRaysWithoutBlockers) {
    const Bitboard expected =
        (1ULL << D1) | (1ULL << D2) | (1ULL << D3) | (1ULL << D5) |
        (1ULL << D6) | (1ULL << D7) | (1ULL << D8) |
        (1ULL << A4) | (1ULL << B4) | (1ULL << C4) |
        (1ULL << E4) | (1ULL << F4) | (1ULL << G4) | (1ULL << H4);

    EXPECT_THAT(attacks_.get_rook_attacks(D4, 0ULL), Eq(expected));
}

TEST_F(AttacksTest, RookAttacksStopAtFirstBlockerInEachDirection) {
    const Bitboard occupied = (1ULL << D6) | (1ULL << F4) | (1ULL << D2) | (1ULL << B4);
    const Bitboard expected =
        (1ULL << D5) | (1ULL << D6) |
        (1ULL << D3) | (1ULL << D2) |
        (1ULL << E4) | (1ULL << F4) |
        (1ULL << C4) | (1ULL << B4);

    EXPECT_THAT(attacks_.get_rook_attacks(D4, occupied), Eq(expected));
}

TEST_F(AttacksTest, BishopAttacksCoverDiagonalsWithoutBlockers) {
    const Bitboard expected =
        (1ULL << E5) | (1ULL << F6) | (1ULL << G7) | (1ULL << H8) |
        (1ULL << C5) | (1ULL << B6) | (1ULL << A7) |
        (1ULL << E3) | (1ULL << F2) | (1ULL << G1) |
        (1ULL << C3) | (1ULL << B2) | (1ULL << A1);

    EXPECT_THAT(attacks_.get_bishop_attacks(D4, 0ULL), Eq(expected));
}

TEST_F(AttacksTest, BishopAttacksStopAtFirstBlockerInEachDirection) {
    const Bitboard occupied = (1ULL << F6) | (1ULL << B6) | (1ULL << F2) | (1ULL << B2);
    const Bitboard expected =
        (1ULL << E5) | (1ULL << F6) |
        (1ULL << C5) | (1ULL << B6) |
        (1ULL << E3) | (1ULL << F2) |
        (1ULL << C3) | (1ULL << B2);

    EXPECT_THAT(attacks_.get_bishop_attacks(D4, occupied), Eq(expected));
}

TEST_F(AttacksTest, QueenAttacksAreUnionOfRookAndBishopAttacks) {
    const Bitboard occupied = (1ULL << D6) | (1ULL << B4) | (1ULL << F2);

    const Bitboard expected =
        attacks_.get_rook_attacks(D4, occupied) |
        attacks_.get_bishop_attacks(D4, occupied);

    EXPECT_THAT(attacks_.get_queen_attacks(D4, occupied), Eq(expected));
}

} // namespace
} // namespace Rmagician
