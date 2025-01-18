#include "earley_algorithm.hh"

std::optional<Tree> generateParseTree(const std::vector<Token> &strippedInput, const State &state) {
    std::vector<Tree> subtrees;
    subtrees.reserve(state.p.rhs.size());
    std::size_t pos = state.left;
    for (std::size_t i = 0; i < state.p.rhs.size(); i++) {
        if (std::holds_alternative<Terminals>(state.p.rhs[i])) {
            subtrees.push_back(Token(strippedInput.at(pos)));
            pos++;
        } else {
            auto it = std::find_if(state.backpointer.begin(), state.backpointer.end(), [&](const typename State::BackPointer &bp) { return bp.dot == i; });
            if (it != state.backpointer.end()) {
                if (std::optional<Tree> res = generateParseTree(strippedInput, it->state)) {
                    subtrees.push_back(*res); 
                } else {
                    return std::nullopt;
                }
            } else {
                return std::nullopt;
            }
            pos = it->state.right;
        }
    }
    return Branch(state.p.lhs, state.left, state.right, subtrees);
};
