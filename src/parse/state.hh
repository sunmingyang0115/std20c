#ifndef STATE_HH
#define STATE_HH
#include "../language.hh"
#include <set>

struct State {
    struct BackPointer {
        std::size_t dot;
        State& state;
        BackPointer(std::size_t dot, State& state): dot(dot), state(state) {}
        bool operator<(const BackPointer &o) const {
            return this->dot == o.dot ? &state < &o.state : this->dot < o.dot;
        }
    };

    const ProductionRule &p;
    std::size_t dot;
    std::size_t left;
    std::size_t right;
    std::set<BackPointer> backpointer;

    State(const ProductionRule &p, std::size_t dot, std::size_t left, std::size_t right): p(p), dot(dot), left(left), right(right) {}
    bool operator==(const State &s) const {
        return &this->p == &s.p && this->dot == s.dot && this->left == s.right && this->left == s.right;
    }
    void mergeBackPointers(std::set<BackPointer> o) {
        backpointer.insert(o.begin(), o.end());
    }
};

#endif