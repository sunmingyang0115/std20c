#ifndef VECTOR_UTIL
#define VECTOR_UTIL
#include <vector>
#include <tuple>
namespace {
    template<std::size_t N, typename T, std::size_t... I>
    auto vectorViewImpl(std::vector<T> &vec, std::index_sequence<I...>) {
        return std::tie(vec[I]...);
    }
}

template<std::size_t N, typename T>
auto vectorView(std::vector<T> &vec) {
    assert((vec.size() >= N && "vectorView: vector has not enough elements"));
    return vectorViewImpl<N>(vec, std::make_index_sequence<N>{});
}



#endif