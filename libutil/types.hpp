#ifndef LIB_UTIL_TYPES_HPP_
#define LIB_UTIL_TYPES_HPP_

/** @file types.hpp
 *  @brief Types utilities
 */

#include <type_traits>

namespace util {
// Get from  https://stackoverflow.com/a/33083231
// Credits to :
// - R. Martinho Fernandes
// (https://stackoverflow.com/users/46642/r-martinho-fernandes)
// - Class Skeleton (https://stackoverflow.com/users/3387453/class-skeleton)
template <typename E>
constexpr auto to_underlying(E e) noexcept {
  return static_cast<std::underlying_type_t<E>>(e);
}
}  // namespace util

#endif  // LIB_UTIL_TYPES_HPP_
