#ifndef EARLEY_ALGORITHM_HH
#define EARLEY_ALGORITHM_HH
#include "chart.hh"
#include "../error_message.hh"
#include <optional>

inline bool skipTokenPredicate(const Token& token) {
    return token.kind == Terminals::SPACE || token.kind == Terminals::COMMENT;
}

Chart generateEarleyChart(const std::vector<Token> &strippedInput);
std::optional<Tree> generateParseTree(const std::vector<Token> &strippedInput, const State &state);
CompilerError generateParseError(const State &lastValidState, const std::vector<Token> &strippedInput);

#endif