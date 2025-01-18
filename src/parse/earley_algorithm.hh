#ifndef EARLEY_ALGORITHM_H
#define EARLEY_ALGORITHM_H
#include "chart.hh"

inline bool skipTokenPredicate(const Token& token) {
    return token.kind == Terminals::SPACE || token.kind == Terminals::COMMENT;
}

Chart generateEarleyChart(const std::vector<Token> &strippedInput);
std::optional<Tree> generateParseTree(const std::vector<Token> &strippedInput, const State &state);
CompilerError generateParseError(const State &lastValidState, const std::vector<Token> &originalInput);

#endif