#ifndef LIB_MODBUS_MODBUS_ADU_HPP_
#define LIB_MODBUS_MODBUS_ADU_HPP_

#include <cstdint>
#include <iostream>
#include <type_traits>

#include <fmt/core.h>
#include <fmt/ostream.h>

#include "modbus-constants.hpp"
#include "modbus-types.hpp"
#include "modbus-utilities.hpp"

namespace modbus {
// forward declarations
namespace internal {
template <constants::function_code modbus_function>
class adu;
}

namespace internal {
/**
 * @brief ADU base class
 *
 * @author  Ray Andrew
 * @ingroup Modbus/Internal
 *
 * ADU structure for TCP:
 * - Header (7 bytes)
 * --- Transaction (2 bytes)
 * --- Protocol (2 bytes)
 * --- Length (2 bytes) [ unit identifier + PDU ]
 * --- Unit (1 byte)
 * - PDU
 * --- Function (1 byte)
 * --- Rest of data... (N byte)
 */
template <constants::function_code modbus_function>
class adu {
 public:
  /**
   * Initializer
   */
  struct initializer_t {
    /**
     * Transaction
     */
    std::uint16_t transaction;
    /**
     * Unit
     */
    std::uint16_t unit;
  };

  /**
   * ADU constructor
   */
  explicit adu() noexcept;

  /**
   * ADU constructor
   *
   * @param transaction transaction id
   * @param unit        unit id
   */
  explicit adu(std::uint16_t transaction,
               std::uint8_t  unit) noexcept;

  /**
   * ADU constructor
   *
   * @param initializer initializer
   */
  explicit adu(const initializer_t& initializer) noexcept;

  /**
   * ADU constructor
   *
   * @param m_header  header struct
   */
  explicit adu(const header_t& m_header) noexcept;

 public:
  /**
   * Encode packet
   *
   * @return packet that has been encoded
   */
  virtual packet_t encode() = 0;

  /**
   * Decode packet
   *
   * @param packet packet to be decoded
   */
  virtual void decode(const packet_t& packet) = 0;

 public:
  /** Getter */
  /**
   * Get function code
   *
   * @return function code
   */
  inline constexpr constants::function_code function() const {
    return modbus_function;
  }

  /**
   * Get transaction id
   *
   * @return transaction id
   */
  inline std::uint16_t transaction() const { return transaction_; }

  /**
   * Get length
   *
   * @return length
   */
  inline std::uint16_t length() const { return length_; }

  /**
   * Get unit id
   *
   * @return unit id
   */
  inline std::uint16_t unit() const { return unit_; }

  /**
   * Get header
   *
   * @return header
   */
  inline header_t header() const { return {transaction(), length(), unit()}; }

  /** Setter */

  /**
   * Iniitalize ADU
   *
   * @param initializer initializer
   *
   * @return instance of ADU
   */
  inline adu& initialize(const initializer_t& initializer);

  /**
   * Set header
   *
   * @param m_header  header struct
   *
   * @return instance of ADU
   */
  inline adu& header(const header_t& m_header);

  /**
   * Set transaction id
   *
   * @param new_transaction new transaction id
   *
   * @return instance of ADU
   */
  inline adu& transaction(std::uint16_t new_transaction);

  /**
   * Calculate length of ADU given PDU length
   *
   * @param pdu_length PDU length
   *
   * @return instance of ADU
   */
  inline adu& calc_length(const internal::packet_t::size_type& pdu_length);

  /**
   * Set length of ADU
   *
   * @param new_length new length
   *
   * @return instance of ADU
   */
  inline adu& length(std::uint16_t new_length);

  /**
   * Set unit id
   *
   * @param new_unit new unit id
   *
   * @return instance of ADU
   */
  inline adu& unit(std::uint8_t new_unit);

  /** Operator */
  /**
   * Equality operator
   *
   * @param other other ADU
   *
   * @return true if transaction_id is equal to other transaction_id
   */
  inline bool operator==(const adu& other) const;

  /**
   * Less-than operator
   *
   * @param other other ADU
   *
   * @return true if transaction_id is less than other transaction_id
   */
  inline bool operator<(const adu& other) const;

  /**
   * More-than operator
   *
   * @param  other other ADU
   *
   * @return true if transaction_id is more than other transaction_id
   */
  inline bool operator>(const adu& other) const;

  /**
   * Dump to string
   *
   * @param  os  ostream
   *
   * @return ostream
   */
  inline virtual std::ostream& dump(std::ostream& os) const;

  /**
   * Ostream operator
   *
   * @param os  ostream
   * @param adu adu instance
   *
   * @return stream
   */
  template <typename ostream>
  inline friend ostream& operator<<(ostream& os, const adu& obj) {
    return obj.dump(os);
  }

 protected:
  /**
   * Get header packet
   *
   * @return packet consists of header
   */
  inline packet_t header_packet();

  /**
   * Response size
   *
   * @param pdu_length PDU length
   *
   * @return ADU length
   */
  inline static constexpr typename internal::packet_t::size_type
  calc_adu_length(const internal::packet_t::size_type& pdu_length);

 protected:
  /**
   * Protocol ID
   */
  static constexpr std::uint16_t protocol = constants::tcp_protocol;
  /**
   * Function code
   */
  static constexpr std::underlying_type_t<constants::function_code>
      function_code = utilities::to_underlying(modbus_function);
  /**
   * Header length
   */
  static constexpr typename internal::packet_t::size_type header_length = 7;
  /**
   * Length index
   */
  static constexpr typename internal::packet_t::size_type length_idx = 4;
  /**
   * Max length
   */
  static constexpr typename internal::packet_t::size_type max_length =
      constants::max_adu_length;
  /**
   * Max PDU length
   */
  static constexpr typename internal::packet_t::size_type max_pdu_size =
      max_length - header_length;

 protected:
  /**
   * Transaction id
   */
  std::uint16_t transaction_;
  /**
   * Length of packet [ unit identifier + PDU ]
   */
  std::uint16_t length_;
  /**
   * Unit id
   */
  std::uint8_t unit_;
};
}  // namespace internal
}  // namespace modbus

#endif  // LIB_MODBUS_MODBUS_ADU_HPP_
