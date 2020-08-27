#ifndef LIB_MODBUS_MODBUS_CONSTANTS_HPP_
#define LIB_MODBUS_MODBUS_CONSTANTS_HPP_

#include <cstdint>
#include <string_view>

namespace modbus {
namespace constants {
/**
 *  \author Ray Andrew
 *  \ingroup Modbus
 *  @brief is a strongly typed enum class representing the function of Modbus
 */
enum class function_code : std::uint8_t {
  min = 0x00,
  read_coils = 0x01,
  read_discrete_inputs = 0x02,
  read_holding_registers = 0x03,
  read_input_registers = 0x04,
  write_single_coil = 0x05,
  write_single_register = 0x06,
  read_exception_status = 0x07,
  diagnostics = 0x08,
  write_multiple_coils = 0x0F,
  write_multiple_registers = 0x10,
  read_file_record = 0x14,
  write_file_record = 0x15,
  mask_write_register = 0x16,
  read_write_multiple_registers = 0x17,
  read_fifo_queue = 0x18,
  encapsulated_interface_transport = 0x2B,
  max = 0x2C,
};

/**
 *  \author Ray Andrew
 *  \ingroup Modbus
 *  @brief is a strongly typed enum class representing the exception of Modbus
 * and internal lib
 */
enum class exception_code : std::uint8_t {
  min = 0x00, /**< helper for checking modbus::exception value */

  /** modbus exception */
  illegal_function = 0x01,
  illegal_data_address,
  illegal_data_value,
  server_device_failure,
  acknowledge,
  server_device_busy,
  negative_acknowledge,
  memory_parity_error,
  undef,
  gateway_path_unavailable,
  gateway_target_device_failed_to_respond,

  /** Internal exception */
  bad_data,      /*<< data is not sent properly, bad request, bad response */
  bad_data_size, /*<< bad data size provided (can be out of bound, buffer size
                    is lesser than expected, etc) */
  connection_problem, /*<< connection problem because because of timed
                         out */
  bad_exception,      /*<< unknown exception */

  /** helper */
  no_exception, /*<< No exception status */
  max           /*<< helper for checking modbus::exeception value */
};

static constexpr std::uint16_t   max_adu_length = 260;
static constexpr std::uint16_t   tcp_protocol = 0x00;
static constexpr std::uint16_t   max_num_bits_read = 0x07D0;
static constexpr std::uint16_t   max_num_regs_read = 0x007D;
static constexpr std::uint16_t   max_num_bits_write = 0x07D0;
static constexpr std::uint16_t   max_num_regs_write = 0x007D;

}  // namespace constants

static constexpr bool check_function(std::uint8_t function) {
  return static_cast<std::uint8_t>(constants::function_code::min) < function &&
         function < static_cast<std::uint8_t>(constants::function_code::max);
}

static constexpr bool check_function(constants::function_code function) {
  return (function != constants::function_code::min) &&
         (function != constants::function_code::max);
}

static constexpr bool check_exception(std::uint8_t exception) {
  return static_cast<std::uint8_t>(constants::exception_code::min) <
             exception &&
         exception < static_cast<std::uint8_t>(constants::exception_code::max);
}

static constexpr bool check_function(constants::exception_code exception) {
  return (exception != constants::exception_code::min) &&
         (exception != constants::exception_code::max);
}
}

#endif  // LIB_MODBUS_MODBUS_CONSTANTS_HPP_
