#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "utils/FenUtils.h"

namespace Rmagician {
namespace {

using ::testing::Eq;

TEST(FenUtilsTest, ParseStartPositionFillsBoardAndMetadata) {
    const std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    Bitboards bb;
    PositionDetails pd;

    ASSERT_TRUE(FenUtils::parse_fen(fen, bb, pd));

    EXPECT_THAT(bb.piece_on(A1), Eq(W_ROOK));
    EXPECT_THAT(bb.piece_on(E1), Eq(W_KING));
    EXPECT_THAT(bb.piece_on(D8), Eq(B_QUEEN));
    EXPECT_THAT(bb.piece_on(H7), Eq(B_PAWN));
    EXPECT_THAT(bb.piece_on(E4), Eq(PIECE_NUM));

    EXPECT_THAT(pd.side_to_move, Eq(WHITE));
    EXPECT_THAT(pd.castling_rights, Eq(static_cast<uint8_t>(WHITE_OO | WHITE_OOO | BLACK_OO | BLACK_OOO)));
    EXPECT_FALSE(pd.en_passant_square.has_value());
    EXPECT_THAT(pd.halfmove_clock, Eq(0));
    EXPECT_THAT(pd.fullmove_number, Eq(1));
}

TEST(FenUtilsTest, ParseFenSetsDefaultsWhenMoveCountersAreMissing) {
    const std::string fen = "8/8/8/8/8/8/8/8 b - -";

    Bitboards bb;
    PositionDetails pd;

    ASSERT_TRUE(FenUtils::parse_fen(fen, bb, pd));

    EXPECT_THAT(pd.side_to_move, Eq(BLACK));
    EXPECT_THAT(pd.castling_rights, Eq(static_cast<uint8_t>(NO_CASTLING)));
    EXPECT_FALSE(pd.en_passant_square.has_value());
    EXPECT_THAT(pd.halfmove_clock, Eq(0));
    EXPECT_THAT(pd.fullmove_number, Eq(1));
}

TEST(FenUtilsTest, ParseFenReadsEnPassantAndSelectedPieces) {
    const std::string fen = "4k3/8/8/3pP3/8/8/8/4K3 w - d6 7 42";

    Bitboards bb;
    PositionDetails pd;

    ASSERT_TRUE(FenUtils::parse_fen(fen, bb, pd));

    ASSERT_TRUE(pd.en_passant_square.has_value());
    EXPECT_THAT(pd.en_passant_square.value(), Eq(D6));
    EXPECT_THAT(pd.halfmove_clock, Eq(7));
    EXPECT_THAT(pd.fullmove_number, Eq(42));

    EXPECT_THAT(bb.piece_on(E8), Eq(B_KING));
    EXPECT_THAT(bb.piece_on(D5), Eq(B_PAWN));
    EXPECT_THAT(bb.piece_on(E5), Eq(W_PAWN));
    EXPECT_THAT(bb.piece_on(E1), Eq(W_KING));
}

TEST(FenUtilsTest, ParseFenRejectsInvalidInputs) {
    Bitboards bb;
    PositionDetails pd;

    EXPECT_FALSE(FenUtils::parse_fen("8/8/8/8/8/8/8/8", bb, pd));
    EXPECT_FALSE(FenUtils::parse_fen("8/8/8/8/8/8/8/8 x - - 0 1", bb, pd));
    EXPECT_FALSE(FenUtils::parse_fen("8/8/8/8/8/8/8/8 w X - 0 1", bb, pd));
    EXPECT_FALSE(FenUtils::parse_fen("8/8/8/8/8/8/8/8 w - z9 0 1", bb, pd));
    EXPECT_FALSE(FenUtils::parse_fen("8/8/8/8/8/8/8/9 w - - 0 1", bb, pd));
    EXPECT_FALSE(FenUtils::parse_fen("8/8/8/8/8/8/8/7Z w - - 0 1", bb, pd));
}

TEST(FenUtilsTest, ToFenBuildsExpectedString) {
    Bitboards bb;
    bb.set_piece(E1, W_KING);
    bb.set_piece(H1, W_ROOK);
    bb.set_piece(A8, B_KING);
    bb.set_piece(B7, B_PAWN);

    PositionDetails pd{};
    pd.side_to_move = BLACK;
    pd.castling_rights = static_cast<uint8_t>(WHITE_OO);
    pd.en_passant_square = E3;
    pd.halfmove_clock = 9;
    pd.fullmove_number = 31;

    const std::string fen = FenUtils::to_fen(bb, pd);
    EXPECT_THAT(fen, Eq("k7/1p6/8/8/8/8/8/4K2R b K e3 9 31"));
}

TEST(FenUtilsTest, ParseFenThenToFenProducesEquivalentStateString) {
    const std::string fen = "r3k2r/pppq1ppp/2npbn2/4p3/2BPP3/2N2N2/PPP2PPP/R1BQ1RK1 b kq - 4 10";

    Bitboards bb;
    PositionDetails pd;

    ASSERT_TRUE(FenUtils::parse_fen(fen, bb, pd));

    const std::string serialized = FenUtils::to_fen(bb, pd);
    EXPECT_THAT(serialized, Eq(fen));
}

} // namespace
} // namespace Rmagician
