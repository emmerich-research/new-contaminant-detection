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
// forward declarations
namespace internal {
packet_t pack_bits(const block::bits::container_type::const_iterator& begin,
                   const block::bits::container_type::const_iterator& end);
block::bits::container_type unpack_bits(const packet_t::const_iterator& begin,
                                        const packet_t::const_iterator& end);
}  // namespace internal

namespace request {
/**
 * base request read bits class
 *
 * @author Ray Andrew
 * @date   August 2020
 *
 * Encode, decode, and execute read coils request
 *
 * Structure :
 * [ (Header...)                ]
 * [ Function (1 byte)          ]
 * [ Starting Address (2 bytes) ]
 * [ Quantity of bits (2 bytes) ]
 */
template <constants::function_code function_code>
class base_read_bits : public internal::request {
 public:
  /**
   * request::base_read_bits constructor
   *
   * @param address address requested
   * @param count   count   requested
   */
  explicit base_read_bits(const address_t&  address = address_t{},
                          const num_bits_t& count = num_bits_t{}) noexcept;

  /**
   * Encode read bits packet from given data
   *
   * @return packet format
   */
  virtual packet_t encode() override;

  /**
   * Decode read bits packet
   *
   * @param data data to be appended
   *
   * @return packet format
   */
  virtual void decode(const packet_t& data) override;

  /**
   * Encode read bits packet
   *
   * @return packet format
   */
  virtual typename internal::response::pointer execute(
      table* data_table) override;

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
   * Number of bits
   */
  num_bits_t count_;
  /**
   * Struct format
   */
  static constexpr std::string_view format = "HH";
};

using read_coils = base_read_bits<constants::function_code::read_coils>;
using read_discrete_inputs =
    base_read_bits<constants::function_code::read_discrete_inputs>;
}  // namespace request

namespace response {
/**
 * base response read bits class
 *
 * @author Ray Andrew
 * @date   August 2020
 *
 * Encode and decode read bits response
 *
 * Structure :
 * [ (Header...)                 ]
 * [ Function (1 byte)           ]
 * [ Byte count = N (1 byte)     ]
 * [ Bits (n = N or N + 1 bytes) ]
 */
template <constants::function_code function_code>
class base_read_bits : public internal::response {
 public:
  /**
   * Create std::unique_ptr of response::read_bits
   *
   * @return std::unique_ptr of response::read_bits
   */
  MAKE_STD_UNIQUE(base_read_bits)

  /**
   * response::read_bits constructor
   *
   * @param request    read coils request pointer
   * @param data_table data table
   */
  explicit base_read_bits(const request::base_read_bits<function_code>* request,
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
  const request::base_read_bits<function_code>* request_;
  /**
   * Slice of data from block of bits from data table
   */
  block::bits::container_type bits_;
  /**
   * Struct format
   */
  static constexpr std::string_view format = "B";
};

using read_coils = base_read_bits<constants::function_code::read_coils>;
using read_discrete_inputs =
    base_read_bits<constants::function_code::read_discrete_inputs>;
}  // namespace response
}

#endif  // LIB_MODBUS_MODBUS_BIT_READ_HPP_
