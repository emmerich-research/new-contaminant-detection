#include "modbus.hpp"

#include "modbus-request.hpp"

namespace modbus {
namespace internal {
request::request(constants::function_code function,
                 const initializer_t&     initializer)
    : adu{function, initializer} {}

request::request(constants::function_code function,
                 std::uint16_t            transaction,
                 std::uint8_t             unit)
    : adu{function, transaction, unit} {}
}  // namespace internal
}  // namespace modbus

