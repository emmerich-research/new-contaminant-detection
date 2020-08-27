#include "modbus.hpp"

#include "modbus-bit-read.hpp"

#include <algorithm>
#include <exception>

#include <struc.hpp>

#include "modbus-exception.hpp"
#include "modbus-logger.hpp"
#include "modbus-utilities.hpp"

namespace modbus {
static packet_t pack_bits(
    const block::bits::container_type::const_iterator& begin,
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

static block::bits::container_type unpack_bits(
    const packet_t::const_iterator& begin,
    const packet_t::const_iterator& end) {
  block::bits::container_type result;

  for (auto ptr = begin; ptr < end; ++ptr) {
    for (int bit = 0x01; bit & 0xff; bit <<= 1) {
      result.push_back((*begin & bit) ? true : false);
    }
  }

  return result;
}

namespace request {
read_coils::read_coils(const address_t&  address,
                       const num_bits_t& count) noexcept
    : request{constants::function_code::read_coils},
      address_{address},
      count_{count} {}

std::uint16_t read_coils::byte_count() const {
  std::uint16_t byte_count = static_cast<std::uint16_t>(count_()) / 8;
  std::uint16_t remainder = static_cast<std::uint16_t>(count_()) % 8;

  if (remainder)
    byte_count++;
  return byte_count;
}

typename packet_t::size_type read_coils::response_size() const {
  return calc_adu_length(1 + byte_count());
}

packet_t read_coils::encode() {
  if (!count_.validate()) {
    throw ex::bad_data();
  }

  calc_length(data_length);
  packet_t packet = header_packet();
  packet.reserve(header_length + data_length + 1);
  packet_t pdu = struc::pack(fmt::format(">{}", format), address_(), count_());
  packet.insert(packet.end(), pdu.begin(), pdu.end());
  return packet;
}

void read_coils::decode(const packet_t& packet) {
  if (packet.at(header_length) !=
      utilities::to_underlying(constants::function_code::read_coils)) {
    throw ex::bad_data();
  }

  decode_header(packet);
  struc::unpack(fmt::format(">{}", format), packet.data() + header_length + 1,
                address_.ref(), count_.ref());
}

typename internal::response::pointer read_coils::execute(table& data_table) {
  if (!count_.validate()) {
    throw ex::illegal_data_value(function(), header());
  }

  if (!data_table.coils().validate(address_, count_)) {
    throw ex::illegal_data_address(function(), header());
  }

  return response::read_coils::create(this, &data_table);
}

std::ostream& read_coils::dump(std::ostream& os) const {
  fmt::print(os,
             "(RequestReadCoils, header[transaction={:#04x}, protocol={:#04x}, "
             "function={:#04x}, unit={:#04x}], pdu[address={:#04x}, count={}])",
             transaction_, protocol, function_code_, unit_, address()(),
             count());
  return os;
}
}

namespace response {
read_coils::read_coils(const request::read_coils* request,
                       table*                     data_table) noexcept
    : response{constants::function_code::read_coils, data_table},
      request_{request} {
  initialize({request_->transaction(), request_->unit()});
}

packet_t read_coils::encode() {
  try {
    const auto& [start, end] =
        data_table_->coils().get(request_->address(), request_->count());

    bits_ = block::bits::container_type{start, end};
    calc_length(request_->byte_count() + 1);
    packet_t packet = header_packet();
    packet.reserve(request_->response_size());
    packet_t pdu =
        struc::pack(fmt::format(">{}", format), request_->byte_count());
    packet_t bits = pack_bits(start, end);
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

std::ostream& read_coils::dump(std::ostream& os) const {
  fmt::print(os,
             "ResponseReadCoils(header[transaction={:#04x}, protocol={:#04x}, "
             "function={:#04x}, unit={:#04x}], pdu[address={:#04x}, "
             "count={}, bits_size={}])",
             transaction_, protocol, function_code_, unit_,
             request_->address()(), request_->count(), bits_.size());
  return os;
}

void read_coils::decode_passed(const packet_t& packet) {
  if (packet.size() != request_->response_size()) {
    throw ex::bad_data();
  }

  packet_t::size_type         byte_idx = header_length + 1;
  block::bits::container_type buffer =
      unpack_bits(packet.begin() + byte_idx + 1, packet.end());

  if (buffer.size() != request_->count().get()) {
    throw ex::bad_data();
  }

  bits_.swap(buffer);
}
}  // namespace response
}  // namespace modbus
