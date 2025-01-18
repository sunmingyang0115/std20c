#ifndef CHART_HH
#define CHART_HH
#include "state.hh"
#include <algorithm>
#include <deque>

class Chart {
    class ChartRow {
        std::deque<State> row;
    public:
        ChartRow() = default;
        ~ChartRow() = default;
        void append(const State &state) {
            typename std::deque<State>::iterator it = std::find(this->row.begin(), this->row.end(), state);
            if (it == this->row.end()) {
                this->row.push_back(state);
            } else {
                it->mergeBackPointers(state.backpointer);
            }   
        }
        State &operator[](std::size_t i) { return row[i]; }
        const State &operator[](std::size_t i) const { return row[i]; }
        std::size_t size() const { return row.size(); }
    };
    std::vector<ChartRow> chart;
public:
    Chart(std::size_t n): chart(n+1) {}
    ~Chart() = default;
    ChartRow &operator[](std::size_t i) { return chart[i]; }
    const ChartRow &operator[](std::size_t i) const { return chart[i]; }
    std::size_t size() const { return chart.size(); }
};

#endif