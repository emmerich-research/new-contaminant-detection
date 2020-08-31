#ifndef LIB_MODBUS_MODBUS_REGISTER_WRITE_HPP_
#define LIB_MODBUS_MODBUS_REGISTER_WRITE_HPP_

#include <cstdint>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <string_view>

#include <fmt/core.h>
#include <fmt/ostream.h>

#include "modbus-constants.hpp"
#include "modbus-types.hpp"

#include "modbus-data-table.hpp"

#include "modbus-adu.hpp"
#include "modbus-request.hpp"
#include "modbus-response.hpp"

namespace modbus {
// forward declarations
namespace request {
class write_single_register;
class write_multiple_registers;
class mask_write_register;
}  // namespace request

namespace response {
class write_single_register;
class write_multiple_registers;
class mask_write_register;
}  // namespace response

namespace request {
/**
 * request write single register class
 *
 * @author Ray Andrew
 * @date   August 2020
 *
 * Encode, decode, and execute read registers request
 *
 * Structure :
 * [ (Header...)                 ]
 * [ Function (1 byte)           ]
 * [ Register Address (2 bytes)  ]
 * [ Register Value (2 bytes)    ]
 */
class write_single_register : public internal::request {
 public:
  /**
   * request::base_read_registers constructor
   *
   * @param address address requested
   * @param count   value   requested
   */
  explicit write_single_register(
      const address_t&   address = address_t{},
      const reg_value_t& value = reg_value_t{}) noexcept;

  /**
   * Encode read registers packet from given data
   *
   * @return packet format
   */
  virtual packet_t encode() override;

  /**
   * Decode read registers packet
   *
   * @param data data to be appended
   *
   * @return packet format
   */
  virtual void decode(const packet_t& data) override;

  /**
   * Encode read registers packet
   *
   * @return packet format
   */
  virtual typename internal::response::pointer execute(
      table* data_table) override;

  /**
   * Get response size for error checking on client
   *
   * @return response size
   */
  inline virtual typename packet_t::size_type response_size() const override;

  /**
   * Get address
   *
   * @return address
   */
  inline const address_t& address() const { return address_; }

  /**
   * Get count
   *
   * @return count
   */
  inline const reg_value_t& value() const {
    return value_;
  }

  /**
   * Dump to string
   *
   * @param  os  ostream
   *
   * @return ostream
   */
  virtual std::ostream& dump(std::ostream& os) const override;

 private:
  /**
   * Data length (4 bytes)
   */
  static constexpr std::uint16_t data_length = 4;
  /**
   * Address
   */
  address_t address_;
  /**
   * Value
   */
  reg_value_t value_;
  /**
   * Struct format
   */
  static constexpr std::string_view format = "HH";
};

/**
 * request write multiple registers class
 *
 * @author Ray Andrew
 * @date   August 2020
 *
 * Encode, decode, and execute write multiple registers request
 *
 * Structure:
 * [ (Header...)                   ]
 * [ Function (1 byte)             ]
 * [ Starting Address (2 bytes)    ]
 * [ Quantity of outputs (2 bytes) ]
 * [ Byte count N (1 byte)         ]
 * [ Output value (N x 1 bytes)    ]
 */
class write_multiple_registers : public internal::request {
 public:
  /**
   * request::write_multiple_registers constructor
   *
   * @param address    output address
   * @param count      count
   * @param values     coil values
   */
  explicit write_multiple_registers(
      const address_t&        address = address_t{},
      const write_num_regs_t& count = write_num_regs_t{},
      std::initializer_list<block::registers::data_type> values = {}) noexcept;

  /**
   *
   * Encode write multiple registers packet from given data
   *
   * @return packet format
   */
  virtual packet_t encode() override;

  /**
   * Decode write multiple registers packet
   *
   * @param data data to be appended
   *
   * @return packet format
   */
  virtual void decode(const packet_t& data) override;

  /**
   * Encode write multiple registers packet
   *
   * @return packet format
   */
  virtual typename internal::response::pointer execute(
      table* data_table) override;

  /**
   * Get response size for error checking on client
   *
   * @return response size
   */
  inline virtual typename packet_t::size_type response_size() const override;

  /**
   * Dump to string
   *
   * @param  os  ostream
   *
   * @return ostream
   */
  virtual std::ostream& dump(std::ostream& os) const override;

 public:
  /**
   * Get address
   *
   * @return address
   */
  inline const address_t& address() const { return address_; }

  /**
   * Get count
   *
   * @return count
   */
  inline const write_num_regs_t& count() const { return count_; }

  /**
   * Byte count
   *
   * @return byte count
   */
  std::uint8_t byte_count() const;

  /**
   * Get value
   *
   * @return value
   */
  inline const block::registers::container_type& values() const {
    return values_;
  }

 private:
  /**
   * Get data length
   *
   * Except Function size + header
   *
   * @return data length
   */
  inline std::uint16_t data_length() const { return 4 + 1 + byte_count_; }

 private:
  /**
   * Address
   */
  address_t address_;
  /**
   * Count
   */
  write_num_regs_t count_;
  /**
   * Byte count
   */
  std::uint8_t byte_count_;
  /**
   * Value
   */
  block::registers::container_type values_;
  /**
   * Struct format
   */
  static constexpr std::string_view format = "HHB";
};
}

namespace response {
/**
 * response write single register class
 *
 * @author Ray Andrew
 * @date   August 2020
 *
 * Structure:
 * [ (Header...)              ]
 * [ Function (1 byte)        ]
 * [ Output Address (2 bytes) ]
 * [ Output Value (2 bytes)   ]
 */
class write_single_register : public internal::response {
 public:
  /**
   * Create std::unique_ptr of response::write_single_register
   *
   * @return std::unique_ptr of response::write_single_register
   */
  MAKE_STD_UNIQUE(write_single_register)

  /**
   * response::write_single_register constructor
   *
   * @param request    write single register request pointer
   * @param data_table data table
   */
  explicit write_single_register(const request::write_single_register* request,
                                 table* data_table = nullptr) noexcept;

  /**
   * Encode packet
   *
   * @return packet format
   */
  virtual packet_t encode() override;

  /**
   * Decode stage passed  packet
   *
   * @param packet packet to parse
   */
  virtual void decode_passed(const packet_t& packet) override;

  /**
   * Get address
   *
   * @return address
   */
  inline const address_t& address() const { return address_; }

  /**
   * Get count
   *
   * @return count
   */
  inline const reg_value_t& value() const { return value_; }

  /**
   * Dump to string
   *
   * @param  os  ostream
   *
   * @return ostream
   */
  virtual std::ostream& dump(std::ostream& os) const override;

 private:
  /**
   * Request pointer
   */
  const request::write_single_register* request_;
  /**
   * Address
   */
  address_t address_;
  /**
   * Value
   */
  reg_value_t value_;
  /**
   * Data length (4 bytes)
   */
  static constexpr std::uint16_t data_length = 4;
  /**
   * Struct format
   */
  static constexpr std::string_view format = "HH";
};

/**
 * response write multiple registers class
 *
 * @author Ray Andrew
 * @date   August 2020
 *
 * Structure:
 * [ (Header...)                   ]
 * [ Function (1 byte)             ]
 * [ Starting Address (2 bytes)    ]
 * [ Quantity of Outputs (2 bytes) ]
 */
class write_multiple_registers : public internal::response {
 public:
  /**
   * Create std::unique_ptr of response::write_multiple_registers
   *
   * @return std::unique_ptr of response::write_multiple_registers
   */
  MAKE_STD_UNIQUE(write_multiple_registers)

  /**
   * response::write_multiple_registers constructor
   *
   * @param request    read registers request pointer
   * @param data_table data table
   */
  explicit write_multiple_registers(
      const request::write_multiple_registers* request,
      table*                                   data_table = nullptr) noexcept;

  /**
   * Encode packet
   *
   * @return packet format
   */
  virtual packet_t encode() override;

  /**
   * Decode stage passed packet
   *
   * @param packet packet to parse
   */
  virtual void decode_passed(const packet_t& packet) override;

  /**
   * Dump to string
   *
   * @param  os  ostream
   *
   * @return ostream
   */
  virtual std::ostream& dump(std::ostream& os) const override;

  /**
   * Get address
   *
   * @return address
   */
  inline const address_t& address() const { return address_; }

  /**
   * Get address
   *
   * @return address
   */
  inline const write_num_regs_t& count() const { return count_; }

 private:
  /**
   * Request pointer
   */
  const request::write_multiple_registers* request_;
  /**
   * Address
   */
  address_t address_;
  /**
   * Count
   */
  write_num_regs_t count_;
  /**
   * Data length (4 bytes)
   */
  static constexpr std::uint16_t data_length = 4;
  /**
   * Struct format
   */
  static constexpr std::string_view format = "HH";
};
}  // namespace response
}  // namespace modbus

#endif  // LIB_MODBUS_MODBUS_REGISTER_WRITE_HPP_
