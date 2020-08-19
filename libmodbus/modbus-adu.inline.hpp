#ifndef LIB_MODBUS_MODBUS_ADU_INLINE_HPP_
#define LIB_MODBUS_MODBUS_ADU_INLINE_HPP_

#include "modbus-adu.hpp"
#include "modbus-utilities.hpp"

namespace modbus {
namespace internal {
template <constants::function_code modbus_function>
adu<modbus_function>::adu() noexcept
    : transaction_{0x00}, length_{0x00}, unit_{0x00} {}

template <constants::function_code modbus_function>
adu<modbus_function>::adu(std::uint16_t transaction, std::uint8_t unit) noexcept
    : transaction_{transaction}, length_{0x00}, unit_{unit} {}

template <constants::function_code modbus_function>
adu<modbus_function>::adu(const initializer_t& initializer) noexcept
    : transaction_{initializer.transaction},
      length_{0x00},
      unit_{initializer.unit} {}

template <constants::function_code modbus_function>
adu<modbus_function>::adu(const header_t& m_header) noexcept
    : transaction_{m_header.transaction},
      length_{m_header.length},
      unit_{m_header.unit} {}

/** Setter */
template <constants::function_code modbus_function>
inline adu<modbus_function>& adu<modbus_function>::header(
    const header_t& m_header) {
  transaction_ = m_header.transaction;
  length_ = m_header.length;
  unit_ = m_header.unit;
  return *this;
}

template <constants::function_code modbus_function>
inline adu<modbus_function>& adu<modbus_function>::initialize(
    const initializer_t& initializer) {
  transaction_ = initializer.transaction;
  length_ = 0x00;
  unit_ = initializer.unit;
  return *this;
}

template <constants::function_code modbus_function>
inline adu<modbus_function>& adu<modbus_function>::transaction(
    std::uint16_t new_transaction) {
  transaction_ = new_transaction;
  return *this;
}

template <constants::function_code modbus_function>
inline adu<modbus_function>& adu<modbus_function>::calc_length(
    const internal::packet_t::size_type& pdu_length) {
  // length = unit id (1 byte) + function code (1 byte) + PDU length
  length_ = 1 + 1 + pdu_length;
  return *this;
}

template <constants::function_code modbus_function>
inline adu<modbus_function>& adu<modbus_function>::length(
    std::uint16_t new_length) {
  length_ = new_length;
  return *this;
}

template <constants::function_code modbus_function>
inline adu<modbus_function>& adu<modbus_function>::unit(std::uint8_t new_unit) {
  unit_ = new_unit;
  return *this;
}

template <constants::function_code modbus_function>
inline bool adu<modbus_function>::operator==(const adu& other) const {
  return transaction_ && other.transaction_;
}

template <constants::function_code modbus_function>
inline bool adu<modbus_function>::operator>(const adu& other) const {
  return transaction_ > other.transaction_;
}

template <constants::function_code modbus_function>
inline bool adu<modbus_function>::operator<(const adu& other) const {
  return transaction_ < other.transaction_;
}

template <constants::function_code modbus_function>
inline packet_t adu<modbus_function>::header_packet() {
  packet_t packet;
  packet.reserve(header_length);
  utilities::pack(packet, transaction());
  utilities::pack(packet, protocol);
  utilities::pack(packet, length());
  utilities::pack(packet, unit());
  return packet;
}

template <constants::function_code modbus_function>
inline constexpr typename internal::packet_t::size_type
adu<modbus_function>::calc_adu_length(
    const internal::packet_t::size_type& data_length) {
  return header_length + 1 + 1 + data_length;
}

template <constants::function_code modbus_function>
inline std::ostream& adu<modbus_function>::dump(std::ostream& os) const {
  fmt::print(os,
             "ADU(header[transaction={:#04x}, protocol={:#04x}, "
             "function={:#04x}, unit={:#04x}])",
             transaction_, protocol, function_code, unit_);
  return os;
}
}  // namespace internal
}  // namespace modbus

#endif  // LIB_MODBUS_MODBUS_ADU_INLINE_HPP_
