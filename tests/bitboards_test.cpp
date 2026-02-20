#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Bitboards.h"

namespace Rmagician {
namespace {

using ::testing::Eq;

TEST(BitboardsTest, ConstructorCreatesEmptyBoard) {
    Bitboards bb;

    EXPECT_THAT(bb.all(), Eq(0ULL));
    EXPECT_THAT(bb.pieces(WHITE), Eq(0ULL));
    EXPECT_THAT(bb.pieces(BLACK), Eq(0ULL));
    EXPECT_THAT(bb.pieces(PAWN, WHITE), Eq(0ULL));
    EXPECT_THAT(bb.pieces(PAWN, BLACK), Eq(0ULL));

    EXPECT_THAT(bb.piece_on(A1), Eq(PIECE_NUM));
    EXPECT_THAT(bb.piece_on(E4), Eq(PIECE_NUM));
    EXPECT_THAT(bb.piece_on(H8), Eq(PIECE_NUM));
}

TEST(BitboardsTest, SetPieceUpdatesBitboardsAndBoardState) {
    Bitboards bb;
    const Bitboard c3 = 1ULL << C3;
    const Bitboard h8 = 1ULL << H8;

    bb.set_piece(C3, W_KNIGHT);
    bb.set_piece(H8, B_ROOK);

    EXPECT_THAT(bb.piece_on(C3), Eq(W_KNIGHT));
    EXPECT_THAT(bb.piece_on(H8), Eq(B_ROOK));
    EXPECT_THAT(bb.piece_on(A1), Eq(PIECE_NUM));

    EXPECT_THAT(bb.pieces(KNIGHT, WHITE), Eq(c3));
    EXPECT_THAT(bb.pieces(ROOK, BLACK), Eq(h8));
    EXPECT_THAT(bb.pieces(WHITE), Eq(c3));
    EXPECT_THAT(bb.pieces(BLACK), Eq(h8));
    EXPECT_THAT(bb.all(), Eq(c3 | h8));
}

TEST(BitboardsTest, RemovePieceClearsAllRelevantBitboards) {
    Bitboards bb;
    const Bitboard d5 = 1ULL << D5;

    bb.set_piece(D5, B_BISHOP);
    bb.remove_piece(D5);

    EXPECT_THAT(bb.piece_on(D5), Eq(PIECE_NUM));
    EXPECT_THAT(bb.pieces(BISHOP, BLACK), Eq(0ULL));
    EXPECT_THAT(bb.pieces(BLACK), Eq(0ULL));
    EXPECT_THAT(bb.all(), Eq(0ULL));
    EXPECT_THAT(d5 & bb.all(), Eq(0ULL));
}

TEST(BitboardsTest, RemovePieceOnEmptySquareDoesNothing) {
    Bitboards bb;
    const Bitboard b2 = 1ULL << B2;
    const Bitboard g7 = 1ULL << G7;

    bb.set_piece(B2, W_PAWN);
    bb.set_piece(G7, B_KING);
    bb.remove_piece(E4);

    EXPECT_THAT(bb.piece_on(B2), Eq(W_PAWN));
    EXPECT_THAT(bb.piece_on(G7), Eq(B_KING));
    EXPECT_THAT(bb.pieces(PAWN, WHITE), Eq(b2));
    EXPECT_THAT(bb.pieces(KING, BLACK), Eq(g7));
    EXPECT_THAT(bb.all(), Eq(b2 | g7));
}

TEST(BitboardsTest, MovePieceMovesToEmptySquare) {
    Bitboards bb;
    const Bitboard g1 = 1ULL << G1;
    const Bitboard f3 = 1ULL << F3;

    bb.set_piece(G1, W_KNIGHT);
    bb.move_piece(G1, F3);

    EXPECT_THAT(bb.piece_on(G1), Eq(PIECE_NUM));
    EXPECT_THAT(bb.piece_on(F3), Eq(W_KNIGHT));
    EXPECT_THAT(bb.pieces(KNIGHT, WHITE), Eq(f3));
    EXPECT_THAT(bb.pieces(WHITE), Eq(f3));
    EXPECT_THAT(bb.all(), Eq(f3));
    EXPECT_THAT(bb.all() & g1, Eq(0ULL));
}

TEST(BitboardsTest, MovePieceCapturesDestinationPiece) {
    Bitboards bb;
    const Bitboard a1 = 1ULL << A1;
    const Bitboard a8 = 1ULL << A8;
    const Bitboard e1 = 1ULL << E1;

    bb.set_piece(A1, W_ROOK);
    bb.set_piece(E1, W_KING);
    bb.set_piece(A8, B_KNIGHT);

    bb.move_piece(A1, A8);

    EXPECT_THAT(bb.piece_on(A1), Eq(PIECE_NUM));
    EXPECT_THAT(bb.piece_on(A8), Eq(W_ROOK));
    EXPECT_THAT(bb.piece_on(E1), Eq(W_KING));

    EXPECT_THAT(bb.pieces(ROOK, WHITE), Eq(a8));
    EXPECT_THAT(bb.pieces(KNIGHT, BLACK), Eq(0ULL));
    EXPECT_THAT(bb.pieces(WHITE), Eq(a8 | e1));
    EXPECT_THAT(bb.pieces(BLACK), Eq(0ULL));
    EXPECT_THAT(bb.all(), Eq(a8 | e1));
    EXPECT_THAT(bb.all() & a1, Eq(0ULL));
}

} // namespace
} // namespace Rmagician
