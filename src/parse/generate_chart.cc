#include "earley_algorithm.hh"

Chart generateEarleyChart(const std::vector<Token> &input) {
    Chart S(input.size());

    auto predict = [&](State &state) {
        for (const auto &p: grammar) {
            State state2(p, 0, state.right, state.right);
            if (p.lhs == std::get<NonTerminals>(state.p.rhs[state.dot])) {
                S[state.right].append(state2);
            }
        }
    };
    auto scan = [&](State &state, Token token) {
        if (std::holds_alternative<Terminals>(state.p.rhs[state.dot]) && std::get<Terminals>(state.p.rhs[state.dot]) == token.kind) {
            State state2(state.p, state.dot + 1, state.left, state.right+1);
            state2.backpointer = state.backpointer;
            S[state.right + 1].append(state2);
        }
    };
    auto complete = [&](State &state) {
        for (std::size_t i = 0; i < S[state.left].size(); i++) {
            const auto &existingState = S[state.left][i];
            if (existingState.dot < existingState.p.rhs.size() && std::holds_alternative<NonTerminals>(existingState.p.rhs[existingState.dot]) && std::get<NonTerminals>(existingState.p.rhs[existingState.dot]) == state.p.lhs) {
                State state2(existingState.p, existingState.dot + 1, existingState.left, state.right);
                state2.backpointer = existingState.backpointer;
                state2.backpointer.insert(typename State::BackPointer(existingState.dot, state));
                S[state.right].append(state2);
            }
        }
    };

    S[0].append(State(grammar[0], 0, 0, 0));
    for (std::size_t k = 0; k <= input.size(); k++) {
        for (size_t i = 0; i < S[k].size(); i++) {
            auto &state = S[k][i];
            if (state.dot < state.p.rhs.size()) {
                auto nextE = state.p.rhs[state.dot];
                if (std::holds_alternative<NonTerminals>(nextE)) {
                    predict(state);
                } else {
                    if (k < input.size()) scan(state, input.at(k));
                }
            } else {
                complete(state);
            }
        }
    }

    return S;
}