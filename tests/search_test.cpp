#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Search.h"

namespace Rmagician {
namespace {

using ::testing::Eq;

bool is_pseudo_legal_move(const Position& position, const Move& move) {
    MoveGenerator generator;
    MoveList list;
    generator.generate_moves(position, list);

    for (int i = 0; i < list.count; ++i) {
        const Move& candidate = list.moves[i];
        if (candidate.from() == move.from() &&
            candidate.to() == move.to() &&
            candidate.flag() == move.flag() &&
            candidate.promotion_piece() == move.promotion_piece()) {
            return true;
        }
    }

    return false;
}

bool is_legal_move(Position& position, const Move& move) {
    if (!is_pseudo_legal_move(position, move)) {
        return false;
    }

    const Color us = position.side_to_move();
    const Color them = (us == WHITE) ? BLACK : WHITE;

    Move copy = move;
    UndoInfo undo{};
    position.make_move(copy, undo);
    const bool legal = !position.is_square_attacked(position.king_square(us), them);
    position.undo_move(copy, undo);

    return legal;
}

TEST(SearchTest, StartPositionReturnsNonEmptyLegalMove) {
    Position position;
    Search search;

    position.set_start_position();
    Move best = search.find_best_move(position, Search::SearchParams{3});

    EXPECT_FALSE(best.is_none());
    EXPECT_TRUE(is_legal_move(position, best));
}

TEST(SearchTest, ChoosesLargeMaterialCaptureWhenAvailable) {
    Position position;
    Search search;

    ASSERT_TRUE(position.set_from_fen("4k3/q7/8/8/8/8/R7/4K3 w - - 0 1"));

    const Move best = search.find_best_move(position, Search::SearchParams{2});

    EXPECT_THAT(best.from(), Eq(A2));
    EXPECT_THAT(best.to(), Eq(A7));
    EXPECT_THAT(best.flag(), Eq(CAPTURE));
}

TEST(SearchTest, BlackAlsoChoosesLargeMaterialCaptureWhenAvailable) {
    Position position;
    Search search;

    ASSERT_TRUE(position.set_from_fen("4k3/8/8/8/8/8/7q/4K2R b - - 0 1"));

    const Move best = search.find_best_move(position, Search::SearchParams{2});

    EXPECT_THAT(best.from(), Eq(H2));
    EXPECT_THAT(best.to(), Eq(H1));
    EXPECT_THAT(best.flag(), Eq(CAPTURE));
}

TEST(SearchTest, PrefersQueenPromotionInSimplePromotionRace) {
    Position position;
    Search search;

    ASSERT_TRUE(position.set_from_fen("6k1/4P3/8/8/8/8/8/4K3 w - - 0 1"));

    const Move best = search.find_best_move(position, Search::SearchParams{2});

    EXPECT_THAT(best.from(), Eq(E7));
    EXPECT_THAT(best.to(), Eq(E8));
    EXPECT_THAT(best.flag(), Eq(PROMOTION));
    EXPECT_THAT(best.promotion_piece(), Eq(W_QUEEN));
}

TEST(SearchTest, ReturnedMoveIsLegalInPinnedPieceScenario) {
    Position position;
    Search search;

    ASSERT_TRUE(position.set_from_fen("4r1k1/8/8/8/8/8/4R2q/4K3 w - - 0 1"));

    const Move best = search.find_best_move(position, Search::SearchParams{2});

    EXPECT_FALSE(best.is_none());
    EXPECT_TRUE(is_legal_move(position, best));
}

TEST(SearchTest, StalematePositionReturnsNoMove) {
    Position position;
    Search search;

    ASSERT_TRUE(position.set_from_fen("7k/5Q2/6K1/8/8/8/8/8 b - - 0 1"));

    const Move best = search.find_best_move(position, Search::SearchParams{2});

    EXPECT_TRUE(best.is_none());
}

TEST(SearchTest, SideWithNoPiecesReturnsNoMove) {
    Position position;
    Search search;

    ASSERT_TRUE(position.set_from_fen("4K3/8/8/8/8/8/8/8 b - - 0 1"));

    const Move best = search.find_best_move(position, Search::SearchParams{2});

    EXPECT_TRUE(best.is_none());
}

} // namespace
} // namespace Rmagician
