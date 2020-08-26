#ifndef LIB_MODBUS_MODBUS_DATA_TABLE_INLINE_HPP_
#define LIB_MODBUS_MODBUS_DATA_TABLE_INLINE_HPP_

#include "modbus-data-table.hpp"

#include <algorithm>
#include <iostream>
#include <iterator>

#include "modbus-exception.hpp"

namespace modbus {
namespace block {
/** base block */
template <template <class...> class base_container_t,
          typename data_t,
          typename count_t>
inline constexpr base<base_container_t, data_t, count_t>::base(
    const address_t& starting_address,
    size_type        capacity,
    data_t           default_value) noexcept
    : starting_address_{std::move(starting_address)},
      container_(capacity, default_value),
      capacity_{capacity},
      default_value_{default_value} {}

template <template <class...> class base_container_t,
          typename data_t,
          typename count_t>
inline constexpr base<base_container_t, data_t, count_t>::base(
    const address_t&      starting_address,
    const container_type& container) noexcept
    : starting_address_{std::move(starting_address)},
      container_(std::move(container)),
      capacity_{container.size()},
      default_value_{0} {}

template <template <class...> class base_container_t,
          typename data_t,
          typename count_t>
inline constexpr bool base<base_container_t, data_t, count_t>::validate(
    const address_t& address,
    const count_t&   count) const {
  if (!count_t::validate(count()))
    return false;

  return (starting_address() <= address) &&
         ((starting_address_() + capacity()) >= (address() + count()));
}

template <template <class...> class base_container_t,
          typename data_t,
          typename count_t>
inline constexpr bool base<base_container_t, data_t, count_t>::validate_sz(
    const address_t& address,
    size_type        count) const {
  return (starting_address() <= address) &&
         ((starting_address_() + capacity()) >= (address() + count));
}

/** sequential block */
template <typename data_t, typename count_t>
inline constexpr sequential<data_t, count_t>::sequential(
    const address_t& starting_address,
    size_type        capacity,
    data_t           default_value) noexcept
    : base<std::vector, data_t, count_t>{starting_address, capacity,
                                         default_value} {
  container().resize(capacity);
}

template <typename data_t, typename count_t>
inline constexpr sequential<data_t, count_t>::sequential(
    const sequential<data_t, count_t>::initializer_t& initializer) noexcept
    : base<std::vector, data_t, count_t>{initializer.starting_address,
                                         initializer.capacity,
                                         initializer.default_value} {
  container().resize(capacity());
}

template <typename data_t, typename count_t>
inline constexpr sequential<data_t, count_t>::sequential(
    const address_t&      starting_address,
    const container_type& container) noexcept
    : base<std::vector, data_t, count_t>{starting_address, container} {}

template <typename data_t, typename count_t>
inline constexpr typename sequential<data_t, count_t>::slice_type
sequential<data_t, count_t>::get(const address_t& address,
                                 const count_t&   count) const {
  if (!validate(address, count)) {
    throw ex::illegal_data_address();
  }

  return {container().begin() + address(),
          container().begin() + address() + count()};
}

template <typename data_t, typename count_t>
inline constexpr void sequential<data_t, count_t>::set(
    const address_t&      address,
    const container_type& buffer) {
  if (!validate_sz(address, buffer.size())) {
    throw ex::illegal_data_address{};
  }

  address_t idx = address - starting_address();
  std::transform(buffer.begin(), buffer.end(), container().begin() + idx(),
                 [](const auto& data) -> data_t { return data; });
}

template <typename data_t, typename count_t>
inline constexpr void sequential<data_t, count_t>::set(const address_t& address,
                                                       data_t           value) {
  if (!validate(address)) {
    throw ex::illegal_data_address{};
  }

  address_t idx = address - starting_address();
  container()[idx()] = value;
}

template <typename data_t, typename count_t>
inline constexpr void sequential<data_t, count_t>::reset() {
  std::fill(container().begin(), container().end(), default_value());
}
}  // namespace block
}  // namespace modbus

#endif // LIB_MODBUS_MODBUS_DATA_TABLE_INLINE_HPP_

