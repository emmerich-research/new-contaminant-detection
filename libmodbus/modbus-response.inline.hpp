#ifndef LIB_MODBUS_MODBUS_RESPONSE_INLINE_HPP_
#define LIB_MODBUS_MODBUS_RESPONSE_INLINE_HPP_

#include "modbus-response.hpp"

namespace modbus {
namespace internal {
template <constants::function_code modbus_function>
response<modbus_function>::response(table* data_table) noexcept
    : data_table_{data_table} {}

template <constants::function_code modbus_function>
inline static constexpr bool response<modbus_function>::check_error(
    std::underlying_type_t<constants::function_code> function_code) {
  return function_code > 0x80;
}
}  // namespace internal
}

#endif  // LIB_MODBUS_MODBUS_RESPONSE_INLINE_HPP_
