#ifndef LIB_NETWORKING_MODBUS_HPP_
#define LIB_NETWORKING_MODBUS_HPP_

/** @file modbus.hpp
 *  @brief Modbus implementation
 *
 * Modbus Implementation
 */

#include <array>
#include <chrono>
#include <cstdint>
#include <functional>
#include <variant>

#include <boost/system/error_code.hpp>

#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <libcore/core.hpp>

NAMESPACE_BEGIN

namespace networking {
// forward declarations
class Modbus;
struct ModbusResponse;
struct ModbusError;

namespace modbus {
/**
 *  \author Ray Andrew
 *  \ingroup Networking
 *  @brief is a strongly typed enum class representing the function of Modbus
 */
enum class function : std::uint8_t {
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
  encapsulated_interface_transport = 0x2B
};

/**
 *  \author Ray Andrew
 *  \ingroup Networking
 *  @brief is a strongly typed enum class representing the exception of Modbus
 * and internal lib
 */
enum class exception : std::uint8_t {
  min_exception = 0x00, /**< helper for checking modbus::eception value */

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
  no_exception, /**<< No exception status */
  max_exception /**<< helper for checking modbus::eception value */
};

enum class phase : std::uint8_t { function, meta, data };

namespace time {
using minutes = std::chrono::minutes;
using seconds = std::chrono::seconds;
}  // namespace time
}  // namespace modbus

/**
 * @brief Modbus Slave implementation.
 *        This is a virtual class wrapper that implement Modbus Slave
 * specification
 *
 * Some of these codes are implemented by Libmodbus
 * (https://github.com/stephane/libmodbus)
 *
 * Modified by Ray Andrew
 * <raydreww@gmail.com> to use Boost ASIO and pure c++ implementation
 *
 * Read Modbus_Application_Protocol_Specification_V1_1b3.pdf for more info
 *
 * @author Ray Andrew
 * @date   July 2020
 */
class Modbus : public StackObj {
 public:
  /**
   * Maximum Protocal Data Unit (PDU) Length
   *
   * Read Modbus_Application_Protocol_Specification_V1_1b3.pdf (chapter 4
   * section 1 page 5)
   */
  static constexpr unsigned int MAX_PDU_LENGTH = 253;
  /**
   * Maximum Application Data Unit (ADU) Length
   *
   * Read Modbus_Application_Protocol_Specification_V1_1b3.pdf (chapter 4
   * section 1 page 5)
   */
  static constexpr unsigned int MAX_ADU_LENGTH = 260;
  /**
   * Maximum Message Length
   *
   * Should be same with MAX_ADU_LENGTH
   */
  static constexpr unsigned int MAX_MESSAGE_LENGTH = MAX_ADU_LENGTH;
  /**
   * Minimum Request Length to be sent
   *
   * This is to determine buffer size for request
   *
   * TCP = 12
   * RTU = 8
   *
   * Choose TCP as the max
   */
  static constexpr unsigned int MIN_REQ_LENGTH = 12;
  /**
   * Max read bits
   *
   * Value: 1 - 2000 (2 bytes, 0x0001 - 0x07D0)
   *
   * Read Modbus_Application_Protocol_Specification_V1_1b3.pdf (chapter 6
   * section 1 page 11)
   */
  static constexpr std::uint16_t MAX_READ_BITS = 0x07D0;
  /**
   * Max write bits
   *
   * Value: 1 - 1968 (2 bytes, 0x0001 - 0x07B0)
   *
   * Read Modbus_Application_Protocol_Specification_V1_1b3.pdf (chapter 6
   * section 11 page 29)
   */
  static constexpr std::uint16_t MAX_WRITE_BITS = 0x07B0;
  /**
   * Max read registers
   *
   * Value: 1 - 125 (2 bytes, 0x0001 - 0x007D)
   *
   * Read Modbus_Application_Protocol_Specification_V1_1b3.pdf (chapter 6
   * section 3 page 15)
   */
  static constexpr std::uint16_t MAX_READ_REGISTERS = 0x007D;
  /**
   * Max write registers
   *
   * Value: 1 - 123 (2 bytes, 0x0001 - 0x007B)
   *
   * Read Modbus_Application_Protocol_Specification_V1_1b3.pdf (chapter 6
   * section 12 page 30)
   */
  static constexpr std::uint16_t MAX_WRITE_REGISTERS = 0x007B;
  /**
   * Max R/W read registers
   *
   * Value: 1 - 125 (2 bytes, 0x0001 - 0x007D)
   *
   * Read Modbus_Application_Protocol_Specification_V1_1b3.pdf (chapter 6
   * section 17 page 38)
   */
  static constexpr std::uint16_t MAX_RW_READ_REGISTERS = 0x007D;
  /**
   * Max R/W write registers
   *
   * Value: 1 - 121 (2 bytes, 0x0001 - 0x0079)
   *
   * Read Modbus_Application_Protocol_Specification_V1_1b3.pdf (chapter 6
   * section 17 page 38)
   */
  static constexpr std::uint16_t MAX_RW_WRITE_REGISTERS = 0x0079;
  /**
   * Modbus buffer
   *
   * Will be max-ed into Modbus::MAX_MESSAGE_LENGTH
   */
  typedef std::array<std::uint8_t, Modbus::MAX_MESSAGE_LENGTH> Buffer;
  /**
   * Modbus buffer for 8 bit
   *
   * Will be max-ed into Modbus::MAX_MESSAGE_LENGTH
   */
  typedef Buffer Buffer8;
  /**
   * Modbus 16 bit buffer
   *
   * Will be max-ed into Modbus::MAX_MESSAGE_LENGTH
   */
  typedef std::array<std::uint16_t, Modbus::MAX_MESSAGE_LENGTH> Buffer16;
  /**
   * Modbus buffer
   *
   * Implementing std::variant with two possibilities, "Good" Response or "Bad"
   * Response (Exception)
   */
  typedef std::variant<ModbusResponse, ModbusError> Response;
  /**
   * Error callback type
   */
  typedef std::function<void(const ModbusError&)> ErrorCallback;
  /**
   * Response callback type
   */
  typedef std::function<void(const ModbusResponse&)> ResponseCallback;
  /**
   * Error type
   */
  typedef boost::system::error_code ErrorCode;
  /**
   * Timeout type
   */
  typedef std::chrono::steady_clock::duration Timeout;
  /**
   * Max Timeout
   */
  static const Timeout MaxTimeout;

 public:
  /**
   * Open connection to Modbus server
   */
  virtual ErrorCode connect() = 0;
  /**
   * Close connection to Modbus server
   */
  virtual ErrorCode close() = 0;
  /**
   * Set slave id of Modbus client instance
   *
   * @param slave_id slave id of this instance
   */
  virtual void slave_id(const std::uint8_t& slave_id);
  /**
   * Read booelan status of bits/coils from Modbus remote device
   * Result will be put into the buffer (containing distinct bits for each
   * address)
   *
   * @param address  address to read
   * @param quantity quantity of bits/coils
   * @param buffer   buffer to write
   *
   * @return Modbus::Response contains Response or Error
   */
  Response read_bits(const std::uint16_t& address,
                     const std::uint16_t& quantity,
                     Buffer8&             buffer);
  /**
   * Read input table of bits/coils from Modbus remote device
   * Result will be put into the buffer
   *
   * @param address  address to read
   * @param quantity quantity of bits/coils
   * @param buffer   buffer to write
   *
   * @return Modbus::Response contains Response or Error
   */
  Response read_input_bits(const std::uint16_t& address,
                           const std::uint16_t& quantity,
                           Buffer8&             buffer);
  /**
   * Read holding registers from Modbus remote device
   * Result will be put into the buffer
   *
   * @param address  address to read
   * @param quantity quantity of bits/coils
   * @param buffer   buffer to write
   *
   * @return Modbus::Response contains Response or Error
   */
  Response read_registers(const std::uint16_t& address,
                          const std::uint16_t& quantity,
                          Buffer16&            buffer);
  /**
   * Read input registers from Modbus remote device
   * Result will be put into the buffer
   *
   * @param address  address to read
   * @param quantity quantity of bits/coils
   * @param buffer   buffer to write
   *
   * @return Modbus::Response contains Response or Error
   */
  Response read_input_registers(const std::uint16_t& address,
                                const std::uint16_t& quantity,
                                Buffer16&            buffer);
  /**
   * Write single bit/coil to Modbus remote device
   *
   * @param address  address to write
   * @param value    value to write
   *
   * @return Modbus::Response contains Response or Error
   */
  Response write_bit(const std::uint16_t& address, const bool& value);
  /**
   * Write single bit/coil to Modbus remote device
   *
   * @param address  address to write
   * @param value    value to write
   *
   * @return Modbus::Response contains Response or Error
   */
  Response write_register(const std::uint16_t& address,
                          const std::uint16_t& value);
  /**
   * Write bits/coils to Modbus remote device
   *
   * @param address  address to write
   * @param quantity quantity of address
   * @param value    value to write
   *
   * @return Modbus::Response contains Response or Error
   */
  Response write_bits(const std::uint16_t& address,
                      const std::uint16_t& quantity,
                      const std::uint8_t*  value);
  /**
   * Write registers to Modbus remote device
   *
   * @param address  address to write
   * @param quantity quantity of address
   * @param value    value to write
   *
   * @return Modbus::Response contains Response or Error
   */
  Response write_registers(const std::uint16_t& address,
                           const std::uint16_t& quantity,
                           const std::uint16_t* value);
  /**
   * Write registers to Modbus remote device
   *
   * @param address  address to write
   * @param and_mask and mask
   * @param or_mask  or mask
   *
   * @return Modbus::Response contains Response or Error
   */
  Response mask_write_register(const std::uint16_t& address,
                               const std::uint16_t& and_mask,
                               const std::uint16_t& or_mask);
  /**
   * Write registers to Modbus remote device
   *
   * @param write_address  address to write
   * @param write_quantity quantity of address to write
   * @param value          value to write
   * @param write_address  address to read
   * @param write_quantity quantity of address to read
   * @param buffer         buffer to write
   *
   * @return Modbus::Response contains Response or Error
   */
  Response read_write_registers(const std::uint16_t& write_address,
                                const std::uint16_t& write_quantity,
                                const std::uint16_t* value,
                                const std::uint16_t& read_address,
                                const std::uint16_t& read_quantity,
                                Buffer16&            buffer);
  /**
   * Set Error callback
   *
   * @param error_cb error callback to set
   */
  void error_callback(const ErrorCallback& error_cb);
  /**
   * Set Response callback
   *
   * @param response_cb response callback to set
   */
  void response_callback(const ResponseCallback& response_cb);
  /**
   * Set callback
   *
   * @param response_cb response callback to set
   * @param error_cb    error callback to set
   */
  void callback(const ResponseCallback& response_cb,
                const ErrorCallback&    error_cb);
  /**
   * Set callback
   *
   * @param error_cb    error callback to set
   * @param response_cb response callback to set
   */
  void callback(const ErrorCallback&    error_cb,
                const ResponseCallback& response_cb);
  /**
   * Set connect to server timeout
   *
   * @param timeout timeout to set
   */
  void connect_timeout(const Modbus::Timeout& timeout);
  /**
   * Set response timeout
   *
   * @param timeout timeout to set
   */
  void response_timeout(const Modbus::Timeout& timeout);
  /**
   * Set request timeout
   *
   * @param timeout timeout to set
   */
  void request_timeout(const Modbus::Timeout& timeout);
  /**
   * Check if response is containing ModbusError
   *
   * @return true if contains ModbusError
   */
  static bool error(const Response& response);
  /**
   * Check if response is not containing ModbusError
   *
   * @return false if contains ModbusError
   */
  static bool succeed(const Response& response);
  /**
   * Get right response from variant
   *
   * @return ModbusResponse
   */
  static const ModbusResponse& get_response(const Response& response);
  /**
   * Get right response from variant
   *
   * @return ModbusResponse
   */
  static ModbusResponse& get_response(Response& response);
  /**
   * Get error from variant
   *
   * @return ModbusError
   */
  static const ModbusError& get_error(const Response& response);
  /**
   * Get error from variant
   *
   * @return ModbusError
   */
  static ModbusError& get_error(Response& response);

 protected:
  /**
   * Get header length of packet that will be sent via specific protocol
   *
   * @return header length
   */
  inline const unsigned int& header_length() const { return header_length_; }
  /**
   * Get transaction id
   *
   * @return transaction
   */
  inline const std::uint16_t& transaction_id() const { return transaction_id_; }
  /**
   * Get slave id
   *
   * @return slave id of device
   */
  inline const std::uint8_t& slave_id() const { return slave_id_; }
  /**
   * Get connect to server timeout
   *
   * @return connect to server timeout
   */
  inline const Modbus::Timeout& connect_timeout() const {
    return connect_timeout_;
  }
  /**
   * Get request timeout
   *
   * @return request timeout
   */
  inline const Modbus::Timeout& request_timeout() const {
    return request_timeout_;
  }
  /**
   * Get response timeout
   *
   * @return response timeout
   */
  inline const Modbus::Timeout& response_timeout() const {
    return response_timeout_;
  }
  /**
   * Check quantity of bits/coils or register whether inside domain of [1,
   * max_quantity] or not
   *
   * @param quantity     quantity that its size should be 2 bytes
   * @param max_quantity max quantity that should be allowed
   *
   * @return true if quantity is in range
   */
  static bool check_quantity(const std::uint16_t& quantity,
                             const std::uint16_t& max_quantity);
  /**
   * Check whether function is defined or not
   *
   * @param function function in enum form
   *
   * @return true if function is defined in the Modbus
   */
  static bool check_function(const modbus::function& function);
  /**
   * Check whether function is defined or not
   *
   * @param function function in numerical form
   *
   * @return true if function is defined in the Modbus
   */
  static bool check_function(const std::uint8_t& function);
  /**
   * Check whether exception is defined or not
   *
   * @param exception exception in enum form
   *
   * @return true if exception is defined in the Modbus and internal
   */
  static bool check_exception(const modbus::exception& exception);
  /**
   * Check whether exception is defined or not
   *
   * @param exception exception in numerical form
   *
   * @return true if exception is defined in the Modbus and internal
   */
  static bool check_exception(const std::uint8_t& exception);
  /**
   * Human friendly Modbus Exception
   *
   * @param exception exception in enum form
   *
   * @return Modbus exception message
   */
  static const char* exception_message(const modbus::exception& exception);
  /**
   * Prepare request before sending
   *
   * @param req       message to be sent
   * @param function  Modbus function
   * @param address   destination address
   * @param quantity  quantity of address (can be used as quantity)
   *
   * @return length of packet
   */
  virtual unsigned int build_request(Buffer&                 req,
                                     const modbus::function& function,
                                     const std::uint16_t&    address,
                                     const std::uint16_t&    quantity) = 0;
  /**
   * Send message to Modbus remote device
   *
   * This could be synchronous or asynchronous
   *
   * @param request request to send
   * @param length  length of message
   *
   * @return ModbusResponse / ModbusError
   */
  virtual Response send(Buffer& request, const std::size_t& length) = 0;
  /**
   * Split 16 bit to 8 bit HI/LOW
   *
   * @param buffer     buffer to write
   * @param value      value to split HI / LOW
   * @param start_addr start address of buffer that will be splitted. start_addr
   * will be HI and end_addr (start_addr + 1) will be LOW
   */
  static void uint16_to_uint8(Buffer&              buffer,
                              const std::uint16_t& value,
                              const unsigned int&  start_addr = 0);
  /**
   * Combine 8 bit to 16 bit
   *
   * @param buffer     buffer to read
   * @param start_addr start address of buffer that will be grouped. start_addr
   * will be HI and end_addr (start_addr + 1) will be LOW
   *
   * @return value in 16 bit
   */
  static std::uint16_t uint8_to_uint16(const Buffer&       buffer,
                                       const unsigned int& start_addr = 0);
  /**
   * Check confirmation reply from Modbus server
   *
   *
   * @param request          request that has been sent to server
   * @param response         response that has been received from server
   *
   * @return confirmation error or not
   */
  bool check_confirmation(const Buffer& request, const Buffer& response) const;
  /**
   * Check confirmation reply from Modbus server
   *
   * @param request          request that has been sent to server
   * @param response         response that has been received from server
   * @param response_length  response length
   *
   * @return Modbus exception
   */
  modbus::exception check_exception(const Buffer&      request,
                                    const Buffer&      response,
                                    const std::size_t& response_length) const;
  /**
   *  Calculate length computed from request
   *
   * Read Modbus_Application_Protocol_Specification_V1_1b3.pdf for more info
   *
   *  @param request buffer request
   *
   *  @return length expected from request
   */
  unsigned int calculate_length_from_request(const Buffer& request) const;
  /**
   *  Calculate next response length after phase
   *
   * Read Modbus_Application_Protocol_Specification_V1_1b3.pdf for more info
   *
   *  @param phase    phase of response
   *  @param response response buffer
   *
   *  @return next length to receive
   */
  unsigned int calculate_next_response_length_after(
      const modbus::phase& phase,
      const Buffer&        response) const;
  /**
   * Check whether excepted length from request is equal with response length
   *
   * @param request          request that has been sent to server
   * @param response         response that has been received from server
   * @param response_length  response length
   *
   * @return length if there is no length mismatch
   */
  std::optional<unsigned int> check_length(
      const Buffer&      request,
      const Buffer&      response,
      const std::size_t& response_length) const;

  /**
   * Set Error callback
   *
   * @return error callback
   */
  inline const ErrorCallback& error_callback() const { return error_callback_; }
  /**
   * Set Response callback
   *
   * @return response callback
   */
  inline const ResponseCallback& response_callback() const {
    return response_callback_;
  }
  /**
   * Process bits for read action
   *
   * @param function Modbus function
   * @param address  address to read
   * @param quantity quantity of bits/coils
   * @param buffer   buffer to write
   *
   * @return Modbus::Response contains Response or Error
   */
  Response process_read_bits(const modbus::function& function,
                             const std::uint16_t&    address,
                             const std::uint16_t&    quantity,
                             Buffer8&                buffer);
  /**
   * Process registers for read action
   *
   * @param function Modbus function
   * @param address  address to read
   * @param quantity quantity of bits/coils
   * @param buffer   buffer to write
   *
   * @return Modbus::Response contains Response or Error
   */
  Response process_read_registers(const modbus::function& function,
                                  const std::uint16_t&    address,
                                  const std::uint16_t&    quantity,
                                  Buffer16&               buffer);

  /**
   * Write to specified register / coil
   *
   * @param function Modbus function
   * @param address  address to read
   * @param value    value to write
   *
   * @return Modbus::Response contains Response or Error
   */
  Response process_write_single(const modbus::function& function,
                                const std::uint16_t&    address,
                                const uint16_t&         value);

 protected:
  /**
   * Modbus Constructor
   *
   * @param header_length   header length for specific protocol implementation
   */
  Modbus(const unsigned int& header_length,
         const Timeout&      connect_timeout = MaxTimeout,
         const Timeout&      request_timeout = MaxTimeout,
         const Timeout&      response_timeout = MaxTimeout);
  /**
   * Modbus destructor
   *
   * Close connection and destroy Modbus instance
   */
  virtual ~Modbus();

 protected:
  /**
   * Header Length
   *
   * Header of packet that will be sent via specific protocol
   */
  const unsigned int header_length_;
  /**
   * Transaction ID
   *
   * It should be 2 byte (2^8), after it overflows, it will be resetted to 0
   */
  std::uint16_t transaction_id_;
  /**
   * Slave id
   *
   * Default is 0xFF for server because it is useless to set slave id for server
   * 0x00 is for broadcast -> every message with this slave id must be accepted
   * by client
   *
   * Read Modbus_Messaging_Implementation_Guide_V1_0b.pdf (chapter 4
   * section 4.1.2  page 23)
   */
  std::uint8_t slave_id_;
  /**
   * Error Callback
   */
  ErrorCallback error_callback_;
  /**
   * Response Callback
   */
  ResponseCallback response_callback_;
  /**
   * Connect to server timeout
   */
  Modbus::Timeout connect_timeout_;
  /**
   * Request  timeout
   */
  Modbus::Timeout request_timeout_;
  /**
   * Response  timeout
   */
  Modbus::Timeout response_timeout_;
};

/**
 * @brief Modbus Response wrapper implementation.
 *
 * @author Ray Andrew
 * @date   July 2020
 */
struct ModbusResponse {
  /** Transaction ID */
  const std::uint16_t transaction_id;
  /** Protocol ID */
  const std::uint16_t protocol_id;
  /** Length of response */
  const std::uint16_t length;
  /** Slave ID */
  const std::uint8_t slave_id;
  /** Modbus function */
  const modbus::function function;
  /** Num of bytes */
  unsigned int num_of_bytes;
  /** Raw request */
  const Modbus::Buffer request;
  /** Raw response */
  const Modbus::Buffer response;
  /** Data */
  std::variant<std::monostate, Modbus::Buffer8, Modbus::Buffer16> data;
};

/**
 * @brief Modbus Error wrapper implementation.
 *
 * Struct to Modbus exception and connection problem
 *
 * @author Ray Andrew
 * @date   July 2020
 */
struct ModbusError {
  /** Modbus and internal exception */
  const modbus::exception exception;
  /** Error message */
  const std::string error;
  /** Internal exception */
  const bool internal = false;
};
}  // namespace networking

NAMESPACE_END

#endif  // LIB_NETWORKING_MODBUS_HPP_
