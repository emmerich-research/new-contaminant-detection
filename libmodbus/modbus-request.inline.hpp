#ifndef LIB_MODBUS_MODBUS_REQUEST_INLINE_HPP_
#define LIB_MODBUS_MODBUS_REQUEST_INLINE_HPP_

#include "modbus-request.hpp"

namespace modbus {
namespace internal {
template <constants::function_code modbus_function>
request<modbus_function>::request() : adu<modbus_function>{} {}

template <constants::function_code modbus_function>
request<modbus_function>::request(const initializer_t& initializer)
    : adu<modbus_function>{initializer} {}

template <constants::function_code modbus_function>
request<modbus_function>::request(std::uint16_t transaction, std::uint8_t unit)
    : adu<modbus_function>{transaction, unit} {}
}  // namespace internal
}  // namespace modbus

#endif  // LIB_MODBUS_MODBUS_REQUEST_INLINE_HPP_
