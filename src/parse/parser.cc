#include "parser.hh"
#include "chart.hh"
#include <cassert>
#include <iostream>
#include <variant>
#include <vector>
#include "earley_algorithm.hh"

std::vector<Token> stripInput(const std::vector<Token> &input) {
    auto stripped = input;
    stripped.erase(std::remove_if(stripped.begin(), stripped.end(), skipTokenPredicate), stripped.end());
    return stripped;
}

std::variant<CompilerError, Tree> earleyParser(const std::vector<Token> &originalInput) {
    const auto strippedInput = stripInput(originalInput);
    Chart S = generateEarleyChart(strippedInput);
    

    auto &endState = S[strippedInput.size()];
    for (size_t i = 0; i < endState.size(); i++) {
        auto &state = endState[i];
        if (state.p.lhs == grammar[0].lhs && state.dot == state.p.rhs.size()) {
            auto t = generateParseTree(strippedInput, state);
            assert((t.has_value() && "this should never happen! (unless the earley parser is broken or the grammar is ambiguous)"));
            return *t;
        }
    }

    // final state not reach, now we backtrace the chart until we find a valid program
    for (int trace = strippedInput.size()-1; trace >= 0; trace--) {
        for (size_t i = 0; i < S[trace].size(); i++) {
            auto &state = S[trace][i];
            return generateParseError(state, originalInput);    
        }
    }
    assert(false && "This should never be reached, implementation is incorrect.");
}