#ifndef LIB_MODBUS_MODBUS_RESPONSE_HPP_
#define LIB_MODBUS_MODBUS_RESPONSE_HPP_

#include <type_traits>

#include "modbus-adu.hpp"
#include "modbus-constants.hpp"
#include "modbus-types.hpp"

namespace modbus {
// forward declarations
class table;
namespace internal {
template <constants::function_code modbus_function>
class request;

template <constants::function_code modbus_function>
class response;
}
template <constants::function_code  modbus_function,
          constants::exception_code modbus_exception>
class exception_response;

namespace internal {
template <constants::function_code modbus_function>
class response : public adu<modbus_function> {
 public:
  /**
   * Pointer type
   */
  typedef std::unique_ptr<response> pointer;
  /**
   * Initializer
   */
  using typename adu<modbus_function>::initializer_t;

 protected:
  /**
   * Response constructor
   *
   * @param data_table data table pointer
   */
  explicit response(table* data_table) noexcept;

  /**
   * Check error
   *
   * @param function_code function code
   *
   * @return error if function_code > 0x80
   */
  inline static constexpr bool check_error(
      std::underlying_type_t<constants::function_code> function_code);

 protected:
  /**
   * Data table pointer
   */
  table* data_table_;
};
}  // namespace internal

template <constants::function_code  modbus_function,
          constants::exception_code modbus_exception>
class exception_response : public internal::response<modbus_function> {};
}  // namespace modbus

#endif // LIB_MODBUS_MODBUS_RESPONSE_HPP_
