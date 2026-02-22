#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>
#include <regex>
#include <sstream>
#include <string>

#include "UCI.h"

namespace Rmagician {
namespace {

using ::testing::HasSubstr;
using ::testing::Not;

class ScopedStreamRedirect {
public:
    ScopedStreamRedirect(std::istream& in, std::ostream& out, std::string input)
        : in_(in), out_(out), input_stream_(std::move(input)),
          old_in_buf_(in_.rdbuf(input_stream_.rdbuf())),
          old_out_buf_(out_.rdbuf(output_stream_.rdbuf())) {}

    ~ScopedStreamRedirect() {
        in_.rdbuf(old_in_buf_);
        out_.rdbuf(old_out_buf_);
    }

    std::string output() const {
        return output_stream_.str();
    }

private:
    std::istream& in_;
    std::ostream& out_;
    std::istringstream input_stream_;
    std::ostringstream output_stream_;
    std::streambuf* old_in_buf_;
    std::streambuf* old_out_buf_;
};

std::string run_uci_script(const std::string& script) {
    ScopedStreamRedirect redirect(std::cin, std::cout, script);
    UCI::loop();
    return redirect.output();
}

TEST(UciTest, RepliesToUciAndIsReady) {
    const std::string output = run_uci_script("uci\nisready\nquit\n");

    EXPECT_THAT(output, HasSubstr("id name Rmagician 1.0"));
    EXPECT_THAT(output, HasSubstr("uciok"));
    EXPECT_THAT(output, HasSubstr("readyok"));
}

TEST(UciTest, GoFromStartPositionReturnsUciMove) {
    const std::string output = run_uci_script(
        "position startpos\n"
        "go depth 1\n"
        "quit\n"
    );

    std::smatch match;
    const std::regex bestmove_regex("bestmove ([a-h][1-8][a-h][1-8][qrbn]?)");
    ASSERT_TRUE(std::regex_search(output, match, bestmove_regex));
    EXPECT_THAT(match[1].str(), Not("0000"));
}

TEST(UciTest, PositionWithMovesAcceptedAndSearched) {
    const std::string output = run_uci_script(
        "position startpos moves e2e4 e7e5 g1f3\n"
        "go depth 1\n"
        "quit\n"
    );

    EXPECT_THAT(output, HasSubstr("bestmove "));
}

TEST(UciTest, StalemateReturnsNoMoveToken) {
    const std::string output = run_uci_script(
        "position fen 7k/5Q2/6K1/8/8/8/8/8 b - - 0 1\n"
        "go depth 1\n"
        "quit\n"
    );

    EXPECT_THAT(output, HasSubstr("bestmove 0000"));
}

TEST(UciTest, SupportsPromotionMoveNotation) {
    const std::string output = run_uci_script(
        "position fen 4k3/P7/8/8/8/8/8/4K3 w - - 0 1 moves a7a8q\n"
        "go depth 1\n"
        "quit\n"
    );

    EXPECT_THAT(output, HasSubstr("bestmove "));
}

} // namespace
} // namespace Rmagician
