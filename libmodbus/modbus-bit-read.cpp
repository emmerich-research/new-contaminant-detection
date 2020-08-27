#include "modbus.hpp"

#include "modbus-bit-read.hpp"

#include <algorithm>
#include <exception>

#include <struc.hpp>

#include "modbus-exception.hpp"
#include "modbus-logger.hpp"
#include "modbus-utilities.hpp"

namespace modbus {
namespace internal {
packet_t pack_bits(const block::bits::container_type::const_iterator& begin,
                   const block::bits::container_type::const_iterator& end) {
  packet_t packet;

  char shift = 0;
  char one_byte = 0;

  for (auto ptr = begin; ptr < end; ++ptr) {
    one_byte |= static_cast<char>(*ptr) << shift;
    if (shift == 7) {
      packet.push_back(one_byte);
      one_byte = shift = 0;
    } else {
      shift++;
    }
  }

  if (shift != 0) {
    packet.push_back(one_byte);
  }

  return packet;
}

block::bits::container_type unpack_bits(const packet_t::const_iterator& begin,
                                        const packet_t::const_iterator& end) {
  block::bits::container_type result;

  for (auto ptr = begin; ptr < end; ++ptr) {
    for (int bit = 0x01; bit & 0xff; bit <<= 1) {
      result.push_back((*begin & bit) ? true : false);
    }
  }

  return result;
}
}  // namespace internal

namespace request {
template <>
std::ostream& base_read_bits<constants::function_code::read_coils>::dump(
    std::ostream& os) const {
  fmt::print(os,
             "RequestReadCoils(header[transaction={:#04x}, protocol={:#04x}, "
             "unit={:#04x}], pdu[function={:#04x}, address={:#04x}, count={}])",
             transaction_, protocol, unit_, function_code_, address()(),
             count());
  return os;
}

template <>
typename internal::response::pointer
base_read_bits<constants::function_code::read_coils>::execute(
    table* data_table) {
  if (!count_.validate()) {
    throw ex::illegal_data_value(function(), header());
  }

  if (!data_table->coils().validate(address_, count_)) {
    throw ex::illegal_data_address(function(), header());
  }

  return response::base_read_bits<constants::function_code::read_coils>::create(
      this, data_table);
}

template <>
std::ostream&
base_read_bits<constants::function_code::read_discrete_inputs>::dump(
    std::ostream& os) const {
  fmt::print(
      os,
      "RequestReadDiscreteInputs(header[transaction={:#04x}, protocol={:#04x}, "
      "unit={:#04x}], pdu[function={:#04x}, address={:#04x}, count={}])",
      transaction_, protocol, unit_, function_code_, address()(), count());
  return os;
}

template <>
typename internal::response::pointer
base_read_bits<constants::function_code::read_discrete_inputs>::execute(
    table* data_table) {
  if (!count_.validate()) {
    throw ex::illegal_data_value(function(), header());
  }

  if (!data_table->discrete_inputs().validate(address_, count_)) {
    throw ex::illegal_data_address(function(), header());
  }

  return response::base_read_bits<
      constants::function_code::read_discrete_inputs>::create(this, data_table);
}
}  // namespace request

namespace response {
template <>
std::ostream& base_read_bits<constants::function_code::read_coils>::dump(
    std::ostream& os) const {
  fmt::print(os,
             "ResponseReadCoils(header[transaction={:#04x}, protocol={:#04x}, "
             "unit={:#04x}], pdu[function={:#04x}, address={:#04x}, "
             "count={}, bits_size={}])",
             transaction_, protocol, unit_, function_code_,
             request_->address()(), request_->count(), bits_.size());
  return os;
}

template <>
packet_t base_read_bits<constants::function_code::read_coils>::encode() {
  try {
    const auto& [start, end] =
        data_table_->coils().get(request_->address(), request_->count());

    bits_ = block::bits::container_type{start, end};
    calc_length(request_->byte_count() + 1);
    packet_t packet = header_packet();
    packet.reserve(request_->response_size());
    packet_t pdu =
        struc::pack(fmt::format(">{}", format), request_->byte_count());
    packet_t bits = internal::pack_bits(start, end);
    packet.insert(packet.end(), pdu.begin(), pdu.end());
    packet.insert(packet.end(), bits.begin(), bits.end());

    if (!request_->check_response_packet(packet)) {
      throw ex::server_device_failure(function(), header());
    }

    return packet;
  } catch (const std::out_of_range&) {
    throw ex::illegal_data_address(function(), header());
  } catch (...) {
    throw ex::server_device_failure(function(), header());
  }
}

template <>
std::ostream&
base_read_bits<constants::function_code::read_discrete_inputs>::dump(
    std::ostream& os) const {
  fmt::print(os,
             "ResponseReadDiscreteInputs(header[transaction={:#04x}, "
             "protocol={:#04x}, "
             "unit={:#04x}], pdu[function={:#04x}, address={:#04x}, "
             "count={}, bits_size={}])",
             transaction_, protocol, unit_, function_code_,
             request_->address()(), request_->count(), bits_.size());
  return os;
}

template <>
packet_t
base_read_bits<constants::function_code::read_discrete_inputs>::encode() {
  try {
    const auto& [start, end] = data_table_->discrete_inputs().get(
        request_->address(), request_->count());

    bits_ = block::bits::container_type{start, end};
    calc_length(request_->byte_count() + 1);
    packet_t packet = header_packet();
    packet.reserve(request_->response_size());
    packet_t pdu =
        struc::pack(fmt::format(">{}", format), request_->byte_count());
    packet_t bits = internal::pack_bits(start, end);
    packet.insert(packet.end(), pdu.begin(), pdu.end());
    packet.insert(packet.end(), bits.begin(), bits.end());

    if (!request_->check_response_packet(packet)) {
      throw ex::server_device_failure(function(), header());
    }

    return packet;
  } catch (const std::out_of_range&) {
    throw ex::illegal_data_address(function(), header());
  } catch (...) {
    throw ex::server_device_failure(function(), header());
  }
}
}  // namespace response
}  // namespace modbus
