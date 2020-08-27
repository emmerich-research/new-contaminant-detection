#ifndef LIB_MODBUS_MODBUS_BIT_READ_HPP_
#define LIB_MODBUS_MODBUS_BIT_READ_HPP_

#include <cstdint>
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
namespace request {
class read_coils;
class read_discrete_inputs;
};  // namespace request

namespace response {
class read_coils;
class read_discrete_inputs;
}  // namespace response

namespace request {
/**
 * read coils class
 *
 * @author Ray Andrew
 * @date   August 2020
 *
 * Encode, decode, and execute read coils request
 * [ (Header...) | Starting Address (2 bytes) | Quantity of coils (2 bytes) ]
 */
class read_coils : public internal::request {
 public:
  /**
   * Read coils constructor
   *
   * @param address address requested
   * @param count   count   requested
   */
  explicit read_coils(const address_t&  address = address_t{},
                      const num_bits_t& count = num_bits_t{}) noexcept;

  /**
   * Encode read coils packet from given data
   *
   * @return packet format
   */
  virtual packet_t encode() override;

  /**
   * Decode read coils packet
   *
   * @param data data to be appended
   *
   * @return packet format
   */
  virtual void decode(const packet_t& data) override;

  /**
   * Encode read coils packet
   *
   * [ (Header...) | Starting Address (2 bytes) | Quantity of coils (2 bytes) ]
   *
   * @return packet format
   */
  virtual typename internal::response::pointer execute(
      table& data_table) override;

  /**
   * Byte count
   *
   * @return byte count
   */
  std::uint16_t byte_count() const;

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

 private:
  /**
   * Data length (4 bytes)
   *
   * [ Starting Adress (2 bytes), Quantity of coils (2 bytes) ]
   */
  static constexpr std::uint16_t data_length = 4;
  /**
   * Address
   */
  address_t address_;
  /**
   * Number of bits
   */
  num_bits_t count_;
  /**
   * Struct format
   */
  static constexpr std::string_view format = "HH";

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
  inline const num_bits_t& count() const { return count_; }
};
}  // namespace request

namespace response {
class read_coils : public internal::response {
 public:
  /**
   * Create std::unique_ptr of response::read_coils
   *
   * @return std::unique_ptr of response::read_coils
   */
  MAKE_STD_UNIQUE(read_coils)

  /**
   * response::read_coils constructor
   *
   * @param request    read coils request pointer
   * @param data_table data table
   */
  explicit read_coils(const request::read_coils* request,
                      table*                     data_table = nullptr) noexcept;

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
   * Dump to string
   *
   * @param  os  ostream
   *
   * @return ostream
   */
  virtual std::ostream& dump(std::ostream& os) const override;

  /**
   * Get bits
   */
  inline const block::bits::container_type& bits() const { return bits_; }

 private:
  /**
   * Request pointer
   */
  const request::read_coils* request_;
  /**
   * Slice of data from block of bits from data table
   */
  block::bits::container_type bits_;
  /**
   * Struct format
   */
  static constexpr std::string_view format = "B";
};
}  // namespace response
}

#endif  // LIB_MODBUS_MODBUS_BIT_READ_HPP_
