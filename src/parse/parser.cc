#include "earley_algorithm.hh"
#include <cassert>

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

    // final state not reach, now we backtrace the chart until we find the last completed state

    for (int trace = strippedInput.size(); trace >= 0; trace--) {
        for (int i = S[trace].size()-1; i >= 0; i--) {
            auto &state = S[trace][i];
            if (state.dot != 0 && state.p.rhs.size() != 0) {
                return generateParseError(state, strippedInput);
            }
        }
    }

    // return generateParseError(lastValidState, strippedInput);
    // for (int trace = strippedInput.size(); trace >= 0; trace--) {
    //     for (int i = S[trace].size()-1; i >= 0; i--) {
    //         auto &state = S[trace][i];
    //         auto it = std::find_if(state.backpointer.begin(), state.backpointer.end(), [&](const typename State::BackPointer &bp) { return bp.state == lastValidState; });
    //         if (it != state.backpointer.end()) {
    //             return generateParseError(state, strippedInput);
    //         }
    //     }
    // }
    assert(false && "This should never be reached, implementation is incorrect.");    
}