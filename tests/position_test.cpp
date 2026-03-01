#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Position.h"

namespace Rmagician {
namespace {

using ::testing::Eq;

TEST(PositionTest, SetStartPositionLoadsExpectedFen) {
    Position position;

    position.set_start_position();

    EXPECT_THAT(
        position.to_fen(),
        Eq("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));
}

TEST(PositionTest, SetFromFenRejectsInvalidFen) {
    Position position;

    EXPECT_FALSE(position.set_from_fen("8/8/8/8/8/8/8/8"));
}

TEST(PositionTest, HasKingReflectsBoardState) {
    Position position;

    ASSERT_TRUE(position.set_from_fen("4k3/8/8/8/8/8/8/8 w - - 0 1"));

    EXPECT_TRUE(position.has_king(BLACK));
    EXPECT_FALSE(position.has_king(WHITE));
}

TEST(PositionTest, MakeMoveThenUndoRestoresStateForQuietMove) {
    Position position;
    ASSERT_TRUE(position.set_from_fen("4k3/8/8/8/8/8/4P3/4K3 w - - 0 1"));
    const std::string before = position.to_fen();

    Move move(E2, E3, PIECE_NUM, QUIET);
    UndoInfo undo{};

    position.make_move(move, undo);
    position.undo_move(move, undo);

    EXPECT_THAT(position.to_fen(), Eq(before));
}

TEST(PositionTest, MakeMoveThenUndoRestoresStateForCapture) {
    Position position;
    ASSERT_TRUE(position.set_from_fen("4k3/8/8/8/8/8/4p3/4K3 w - - 0 1"));
    position.bitboards().set_piece(E1, W_KING);
    position.bitboards().set_piece(E2, B_PAWN);
    position.bitboards().set_piece(D1, W_QUEEN);
    const std::string before = position.to_fen();

    Move move(D1, E2, PIECE_NUM, CAPTURE);
    UndoInfo undo{};

    position.make_move(move, undo);
    position.undo_move(move, undo);

    EXPECT_THAT(position.to_fen(), Eq(before));
}

TEST(PositionTest, MakeMoveThenUndoRestoresStateForKingCastle) {
    Position position;
    ASSERT_TRUE(position.set_from_fen("4k3/8/8/8/8/8/8/4K2R w K - 0 1"));
    const std::string before = position.to_fen();

    Move move(E1, G1, PIECE_NUM, KING_CASTLE);
    UndoInfo undo{};

    position.make_move(move, undo);
    EXPECT_THAT(position.bitboards().piece_on(G1), Eq(W_KING));
    EXPECT_THAT(position.bitboards().piece_on(F1), Eq(W_ROOK));

    position.undo_move(move, undo);
    EXPECT_THAT(position.to_fen(), Eq(before));
}

TEST(PositionTest, KingMoveClearsBothCastlingRights) {
    Position position;
    ASSERT_TRUE(position.set_from_fen("4k3/8/8/8/8/8/8/R3K2R w KQ - 0 1"));

    Move move(E1, E2, PIECE_NUM, QUIET);
    UndoInfo undo{};
    position.make_move(move, undo);

    EXPECT_THAT(position.castling_rights(), Eq(0));
}

TEST(PositionTest, RookMoveClearsOnlyCorrespondingCastlingRight) {
    Position position;
    ASSERT_TRUE(position.set_from_fen("4k3/8/8/8/8/8/8/R3K2R w KQ - 0 1"));

    Move move(H1, H2, PIECE_NUM, QUIET);
    UndoInfo undo{};
    position.make_move(move, undo);

    EXPECT_THAT(position.castling_rights(), Eq(WHITE_OOO));
}

TEST(PositionTest, CapturingHomeRookClearsDefenderCastlingRight) {
    Position position;
    ASSERT_TRUE(position.set_from_fen("4k3/8/8/8/8/8/7b/4K2R b K - 0 1"));

    Move move(H2, H1, PIECE_NUM, CAPTURE);
    UndoInfo undo{};
    position.make_move(move, undo);

    EXPECT_THAT(position.castling_rights(), Eq(0));
}

TEST(PositionTest, EnPassantCaptureRemovesPawnAndUndoRestoresState) {
    Position position;
    ASSERT_TRUE(position.set_from_fen("4k3/8/8/3pP3/8/8/8/4K3 w - d6 0 1"));
    const std::string before = position.to_fen();

    Move move(E5, D6, PIECE_NUM, EN_PASSANT);
    UndoInfo undo{};
    position.make_move(move, undo);

    EXPECT_THAT(position.bitboards().piece_on(D6), Eq(W_PAWN));
    EXPECT_THAT(position.bitboards().piece_on(D5), Eq(PIECE_NUM));

    position.undo_move(move, undo);
    EXPECT_THAT(position.to_fen(), Eq(before));
}

TEST(PositionTest, PromotionReplacesPawnWithPromotedPiece) {
    Position position;
    ASSERT_TRUE(position.set_from_fen("4k3/4P3/8/8/8/8/8/4K3 w - - 0 1"));

    Move move(E7, E8, W_QUEEN, PROMOTION);
    UndoInfo undo{};

    position.make_move(move, undo);

    EXPECT_THAT(position.bitboards().piece_on(E8), Eq(W_QUEEN));
    EXPECT_THAT(position.bitboards().piece_on(E7), Eq(PIECE_NUM));
}

TEST(PositionTest, IsSquareAttackedByRookReturnsTrueWhenLineIsOpen) {
    Position position;
    ASSERT_TRUE(position.set_from_fen("4k3/8/8/8/8/8/4r3/4K3 w - - 0 1"));

    EXPECT_TRUE(position.is_square_attacked(E1, BLACK));
    EXPECT_FALSE(position.is_square_attacked(D1, BLACK));
}

TEST(PositionTest, IsSquareAttackedDetectsWhitePawnAttacksFromCenter) {
    Position position;
    ASSERT_TRUE(position.set_from_fen("4k3/8/8/8/3P4/8/8/4K3 w - - 0 1"));

    EXPECT_TRUE(position.is_square_attacked(C5, WHITE));
    EXPECT_TRUE(position.is_square_attacked(E5, WHITE));
    EXPECT_FALSE(position.is_square_attacked(D5, WHITE));
}

TEST(PositionTest, IsSquareAttackedDetectsBlackPawnAttacksFromCenter) {
    Position position;
    ASSERT_TRUE(position.set_from_fen("4k3/8/8/3p4/8/8/8/4K3 w - - 0 1"));

    EXPECT_TRUE(position.is_square_attacked(C4, BLACK));
    EXPECT_TRUE(position.is_square_attacked(E4, BLACK));
    EXPECT_FALSE(position.is_square_attacked(D4, BLACK));
}

TEST(PositionTest, IsSquareAttackedHandlesPawnAttackEdgesWithoutWraparound) {
    Position position;
    ASSERT_TRUE(position.set_from_fen("4k3/8/8/8/P7/8/8/4K3 w - - 0 1"));

    EXPECT_TRUE(position.is_square_attacked(B5, WHITE));
    EXPECT_FALSE(position.is_square_attacked(H5, WHITE));

    ASSERT_TRUE(position.set_from_fen("4k3/8/8/7p/8/8/8/4K3 w - - 0 1"));
    EXPECT_TRUE(position.is_square_attacked(G4, BLACK));
    EXPECT_FALSE(position.is_square_attacked(A4, BLACK));
}

} // namespace
} // namespace Rmagician
