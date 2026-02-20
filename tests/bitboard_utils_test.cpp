#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "utils/BitboardUtils.h"

namespace Rmagician {
namespace {

using ::testing::Eq;
using ::testing::HasSubstr;
using namespace BitboardUtils;

TEST(BitboardUtilsTest, PopcountLsbAndMsbWorkForTypicalBitboards) {
    const Bitboard bb = (1ULL << A1) | (1ULL << C3) | (1ULL << H8);

    EXPECT_THAT(popcount(bb), Eq(3));
    EXPECT_THAT(lsb(bb), Eq(static_cast<int>(A1)));
    EXPECT_THAT(msb(bb), Eq(static_cast<int>(H8)));
}

TEST(BitboardUtilsTest, PopLsbReturnsLeastBitAndMutatesBitboard) {
    Bitboard bb = (1ULL << B2) | (1ULL << E5) | (1ULL << G7);

    EXPECT_THAT(pop_lsb(bb), Eq(B2));
    EXPECT_THAT(bb, Eq((1ULL << E5) | (1ULL << G7)));
    EXPECT_THAT(pop_lsb(bb), Eq(E5));
    EXPECT_THAT(bb, Eq(1ULL << G7));
}

TEST(BitboardUtilsTest, SingleAndMultipleBitChecksAreConsistent) {
    EXPECT_FALSE(is_single_bit(0ULL));
    EXPECT_FALSE(has_multiple_bits(0ULL));

    const Bitboard single = 1ULL << D4;
    EXPECT_TRUE(is_single_bit(single));
    EXPECT_FALSE(has_multiple_bits(single));

    const Bitboard multi = (1ULL << D4) | (1ULL << E4);
    EXPECT_FALSE(is_single_bit(multi));
    EXPECT_TRUE(has_multiple_bits(multi));
}

TEST(BitboardUtilsTest, SquareBitboardHelpersReturnExpectedMasks) {
    EXPECT_THAT(square_bb(E4), Eq(1ULL << E4));
    EXPECT_THAT(square_bb(A1, H8), Eq((1ULL << A1) | (1ULL << H8)));
}

TEST(BitboardUtilsTest, FileRankAndMakeSquareRoundTrip) {
    EXPECT_THAT(file_of(D6), Eq(3));
    EXPECT_THAT(rank_of(D6), Eq(5));
    EXPECT_THAT(make_square(3, 5), Eq(D6));

    EXPECT_THAT(make_square(file_of(B1), rank_of(B1)), Eq(B1));
    EXPECT_THAT(make_square(file_of(H7), rank_of(H7)), Eq(H7));
}

TEST(BitboardUtilsTest, SquareStringConversionSupportsValidAndInvalidInput) {
    EXPECT_THAT(square_to_string(A1), Eq("a1"));
    EXPECT_THAT(square_to_string(E4), Eq("e4"));
    EXPECT_THAT(square_to_string(H8), Eq("h8"));

    EXPECT_THAT(string_to_square("a1"), Eq(A1));
    EXPECT_THAT(string_to_square("e4"), Eq(E4));
    EXPECT_THAT(string_to_square("h8"), Eq(H8));

    EXPECT_THAT(string_to_square(""), Eq(SQUARE_NUM));
    EXPECT_THAT(string_to_square("a"), Eq(SQUARE_NUM));
    EXPECT_THAT(string_to_square("a10"), Eq(SQUARE_NUM));
    EXPECT_THAT(string_to_square("i4"), Eq(SQUARE_NUM));
    EXPECT_THAT(string_to_square("a9"), Eq(SQUARE_NUM));
    EXPECT_THAT(string_to_square("A1"), Eq(SQUARE_NUM));
}

TEST(BitboardUtilsTest, ShiftMovesBitsInGivenDirection) {
    const Bitboard e4 = 1ULL << E4;

    EXPECT_THAT(shift(e4, NORTH), Eq(1ULL << E5));
    EXPECT_THAT(shift(e4, SOUTH), Eq(1ULL << E3));
    EXPECT_THAT(shift(e4, EAST), Eq(1ULL << F4));
    EXPECT_THAT(shift(e4, WEST), Eq(1ULL << D4));
    EXPECT_THAT(shift(e4, NORTH_EAST), Eq(1ULL << F5));
    EXPECT_THAT(shift(e4, NORTH_WEST), Eq(1ULL << D5));
    EXPECT_THAT(shift(e4, SOUTH_EAST), Eq(1ULL << F3));
    EXPECT_THAT(shift(e4, SOUTH_WEST), Eq(1ULL << D3));
}

TEST(BitboardUtilsTest, ShiftPreventsHorizontalWrapAround) {
    const Bitboard h_file = 1ULL << H4;
    const Bitboard a_file = 1ULL << A4;

    EXPECT_THAT(shift(h_file, EAST), Eq(0ULL));
    EXPECT_THAT(shift(h_file, NORTH_EAST), Eq(0ULL));
    EXPECT_THAT(shift(h_file, SOUTH_EAST), Eq(0ULL));

    EXPECT_THAT(shift(a_file, WEST), Eq(0ULL));
    EXPECT_THAT(shift(a_file, NORTH_WEST), Eq(0ULL));
    EXPECT_THAT(shift(a_file, SOUTH_WEST), Eq(0ULL));
}

TEST(BitboardUtilsTest, PrintBitboardWritesHumanReadableOutput) {
    const Bitboard bb = (1ULL << A1) | (1ULL << H8);

    testing::internal::CaptureStdout();
    print_bitboard(bb, "test_board");
    const std::string output = testing::internal::GetCapturedStdout();

    EXPECT_THAT(output, HasSubstr("test_board"));
    EXPECT_THAT(output, HasSubstr("Bitboard: 0x8000000000000001"));
    EXPECT_THAT(output, HasSubstr("Popcount: 2"));
}

} // namespace
} // namespace Rmagician
