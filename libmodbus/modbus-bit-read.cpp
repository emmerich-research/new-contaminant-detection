#include "modbus.hpp"

#include "modbus-bit-read.hpp"

#include <algorithm>

#include <struc.hpp>

#include "modbus-exception.hpp"
#include "modbus-logger.hpp"

namespace modbus {
namespace request {
read_coils::read_coils(const address_t&  address,
                       const num_bits_t& count) noexcept
    : address_{address}, count_{count} {}

internal::packet_t read_coils::encode() {
  calc_length(data_length);
  internal::packet_t packet = header_packet();
  packet.reserve(header_length + data_length + 1);
  internal::packet_t pdu =
      struc::pack(fmt::format(">{}", format), address_(), count_());
  packet.insert(packet.end(), pdu.begin(), pdu.end());
  return packet;
}

void read_coils::decode(const internal::packet_t& packet) {
  if (packet.at(header_length) !=
      utilities::to_underlying(constants::function_code::read_coils)) {
    throw ex::bad_data{};
  }

  decode_header(packet);
  struc::unpack(fmt::format(">{}", format), packet.data() + header_length + 1,
                address_.ref(), count_.ref());
}

typename internal::response<constants::function_code::read_coils>::pointer
read_coils::execute(table& data_table) {
  if (count_() < 1 && count_() > 0x7D0) {
    throw ex::illegal_data_value{};
  }

  return response::read_coils::create_unique(this, &data_table);
}

std::ostream& read_coils::dump(std::ostream& os) const {
  fmt::print(os,
             "(RequestReadCoils, header[transaction={:#04x}, protocol={:#04x}, "
             "function={:#04x}, unit={:#04x}], pdu[address={:#04x}, count={}])",
             transaction_, protocol, function_code, unit_, address()(),
             count());
  return os;
}
}

namespace response {
read_coils::read_coils(const request::read_coils* request,
                       table*                     data_table) noexcept
    : response<constants::function_code::read_coils>{data_table},
      request_{request} {
  initialize({request_->transaction(), request_->unit()});
}

internal::packet_t read_coils::encode() {
  const auto& [start, end] =
      data_table_->coils().get(request_->address(), request_->count());
  bits_ = block::bits::container_type{start, end};

  internal::packet_t packet = header_packet();
  internal::packet_t pdu =
      struc::pack(fmt::format(">{}", format), request_->byte_count());
  internal::packet_t bits(bits_.begin(), bits_.end());
  packet.insert(packet.end(), pdu.begin(), pdu.end());
  packet.insert(packet.end(), bits.begin(), bits.end());
  return packet;
}

std::ostream& read_coils::dump(std::ostream& os) const {
  fmt::print(os,
             "ResponseReadCoils(header[transaction={:#04x}, protocol={:#04x}, "
             "function={:#04x}, unit={:#04x}], pdu[address={:#04x}, "
             "count={}, bits_size={}])",
             transaction_, protocol, function_code, unit_,
             request_->address()(), request_->count(), bits_.size());
  return os;
}

void read_coils::decode(const internal::packet_t& packet) {
  if (packet.at(header_length) !=
      utilities::to_underlying(constants::function_code::read_coils)) {
    throw ex::bad_data{};
  }

  decode_header(packet);
  internal::packet_t::size_type byte_idx = header_length + 1;
  block::bits::container_type   buffer;
  std::transform(
      packet.begin() + byte_idx + 1, packet.end(), std::back_inserter(buffer),
      [](const internal::packet_t::value_type& byte)
          -> block::bits::container_type::value_type {
        return static_cast<block::bits::container_type::value_type>(byte);
      });

  if (buffer.size() != packet.at(byte_idx)) {
    throw ex::bad_data{};
  }

  bits_.swap(buffer);
}
}  // namespace response
}  // namespace modbus
