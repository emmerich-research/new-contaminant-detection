#ifndef LIB_MODBUS_MODBUS_EXCEPTION_HPP_
#define LIB_MODBUS_MODBUS_EXCEPTION_HPP_

#include <exception>
#include <type_traits>

#include "modbus-constants.hpp"
#include "modbus-types.hpp"
#include "modbus-utilities.hpp"

namespace modbus {
namespace ex {
// forward declarations
template <constants::exception_code modbus_exception>
class specification;

template <constants::exception_code modbus_exception>
class internal;

using out_of_range = std::out_of_range;

/** modbus spec exception */
using illegal_function =
    specification<constants::exception_code::illegal_function>;
using illegal_data_address =
    specification<constants::exception_code::illegal_data_address>;
using illegal_data_value =
    specification<constants::exception_code::illegal_data_value>;
using server_device_failure =
    specification<constants::exception_code::server_device_failure>;
using acknowledge = specification<constants::exception_code::acknowledge>;
using server_device_busy =
    specification<constants::exception_code::server_device_busy>;
using memory_parity_error =
    specification<constants::exception_code::memory_parity_error>;
using gateway_path_unavailable =
    specification<constants::exception_code::gateway_path_unavailable>;
using gateway_target_device_failed_to_respond = specification<
    constants::exception_code::gateway_target_device_failed_to_respond>;

/** internal exception */
using bad_data = internal<constants::exception_code::bad_data>;
using bad_data_size = internal<constants::exception_code::bad_data_size>;
using bad_exception = internal<constants::exception_code::bad_exception>;
using no_exception = internal<constants::exception_code::no_exception>;

template <constants::exception_code modbus_exception>
class specification : public std::domain_error {
  static_assert(
      modbus_exception >= constants::exception_code::illegal_function &&
      modbus_exception <=
          constants::exception_code::gateway_target_device_failed_to_respond);

 public:
  /**
   * Specification exception constructor
   */
  specification() : std::domain_error{message()} {}

 private:
  /**
   * Exception message
   *
   * @return exception message
   */
  inline constexpr const char* message() const noexcept {
    if constexpr (modbus_exception ==
                  constants::exception_code::illegal_function) {
      return "Illegal function";
    } else if constexpr (modbus_exception ==
                         constants::exception_code::illegal_data_address) {
      return "Illegal data address";
    } else if constexpr (modbus_exception ==
                         constants::exception_code::illegal_data_value) {
      return "Illegal data value";
    } else if constexpr (modbus_exception ==
                         constants::exception_code::server_device_failure) {
      return "Service device failure";
    } else if constexpr (modbus_exception ==
                         constants::exception_code::acknowledge) {
      return "Acknowledge";
    } else if constexpr (modbus_exception ==
                         constants::exception_code::server_device_busy) {
      return "Server device busy";
    } else if constexpr (modbus_exception ==
                         constants::exception_code::memory_parity_error) {
      return "Memory parity error";
    } else if constexpr (modbus_exception ==
                         constants::exception_code::gateway_path_unavailable) {
      return "Gateway path unavailable";
    } else if constexpr (modbus_exception ==
                         constants::exception_code::
                             gateway_target_device_failed_to_respond) {
      return "Gateway target device failed to respond";
    }
  }
};

template <constants::exception_code modbus_exception>
class internal : public std::domain_error {
  static_assert(modbus_exception >= constants::exception_code::bad_data &&
                modbus_exception <= constants::exception_code::no_exception);

 public:
  /**
   * Internal exception constructor
   */
  internal() : std::domain_error{message()} {}

 private:
  /**
   * Exception message
   *
   * @return exception message
   */
  inline constexpr const char* message() const noexcept {
    if constexpr (modbus_exception == constants::exception_code::bad_data) {
      return "Bad data";
    } else if constexpr (modbus_exception ==
                         constants::exception_code::bad_data_size) {
      return "Bad data size";
    } else if constexpr (modbus_exception ==
                         constants::exception_code::connection_problem) {
      return "Connection problem";
    } else if constexpr (modbus_exception ==
                         constants::exception_code::bad_exception) {
      return "Bad exception";
    } else if constexpr (modbus_exception ==
                         constants::exception_code::no_exception) {
      return "No exception";
    }
  }
};

}  // namespace ex
}

#endif // LIB_MODBUS_MODBUS_EXCEPTION_HPP_


