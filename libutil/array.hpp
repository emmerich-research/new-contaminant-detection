#ifndef LIB_UTIL_ARRAY_HPP_
#define LIB_UTIL_ARRAY_HPP_

/** @file array.hpp
 *  @brief Array utilities
 */

#include <array>
#include <cstdlib>
#include <utility>

namespace util {
// taken from https://stackoverflow.com/a/51269758
template <std::size_t src, typename T, int... I>
std::array<T, sizeof...(I)> get_elements(std::index_sequence<I...>,
                                         std::array<T, src> const& inp) {
  return {inp[I]...};
}

// taken from https://stackoverflow.com/a/51269758
template <int N, typename T, std::size_t src>
auto first_elements(std::array<T, src> const& inp)
    -> decltype(get_elements(std::make_index_sequence<N>{}, inp)) {
  return get_elements(std::make_index_sequence<N>{}, inp);
}
}  // namespace util

#endif  // LIB_UTIL_ARRAY_HPP_
