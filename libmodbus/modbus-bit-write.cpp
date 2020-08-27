#include "modbus.hpp"

#include "modbus-bit-write.hpp"

#include <algorithm>
#include <exception>

#include <struc.hpp>

#include "modbus-exception.hpp"
#include "modbus-logger.hpp"
#include "modbus-utilities.hpp"

namespace modbus {
namespace request {
write_single_coil::write_single_coil(const address_t& address,
                                     value::bits      value) noexcept
    : internal::request{constants::function_code::write_single_coil},
      address_{address},
      value_{value} {}

typename packet_t::size_type write_single_coil::response_size() const {
  return calc_adu_length(data_length);
}

packet_t write_single_coil::encode() {
  if (!address_.validate()) {
    throw ex::bad_data();
  }

  calc_length(data_length);
  packet_t packet = header_packet();
  packet.reserve(header_length + 1 + data_length);
  packet_t pdu = struc::pack(fmt::format(">{}", format), address_(),
                             utilities::to_underlying(value_));
  packet.insert(packet.end(), pdu.begin(), pdu.end());
  return packet;
}

void write_single_coil::decode(const packet_t& packet) {
  try {
    if (packet.at(header_length) != utilities::to_underlying(function())) {
      throw ex::bad_data();
    }

    decode_header(packet);

    std::uint16_t temp;
    struc::unpack(fmt::format(">{}", format), packet.data() + header_length + 1,
                  address_.ref(), temp);

    if (check_bits_value(temp)) {
      value_ = static_cast<value::bits>(temp);
    } else {
      throw ex::bad_data();
    }
  } catch (...) {
    throw ex::server_device_failure(function(), header());
  }
}

typename internal::response::pointer write_single_coil::execute(
    table* data_table) {
  if (!check_bits_value(utilities::to_underlying(value_))) {
    throw ex::illegal_data_value(function(), header());
  }

  if (!data_table->coils().validate(address_)) {
    throw ex::illegal_data_address(function(), header());
  }

  data_table->coils().set(address_, value_ == value::bits::on);

  return response::write_single_coil::create(this, data_table);
}

std::ostream& write_single_coil::dump(std::ostream& os) const {
  fmt::print(
      os,
      "RequestWriteSingleCoil(header[transaction={:#04x}, protocol={:#04x}, "
      "unit={:#04x}], pdu[function={:#04x}, address={:#04x}, "
      "value={:#04x}])",
      transaction_, protocol, unit_, function_code_, address()(),
      utilities::to_underlying(value_));
  return os;
}
}

namespace response {
write_single_coil::write_single_coil(const request::write_single_coil* request,
                                     table* data_table) noexcept
    : internal::response{request->function(), data_table}, request_{request} {
  initialize({request_->transaction(), request_->unit()});
}

packet_t write_single_coil::encode() {
  try {
    const auto& value = data_table_->coils().get(request_->address());

    value_ = value ? value::bits::on : value::bits::off;

    calc_length(data_length);
    packet_t packet = header_packet();
    packet.reserve(request_->response_size());
    packet_t pdu =
        struc::pack(fmt::format(">{}", format), request_->address()(),
                    utilities::to_underlying(value_));
    packet.insert(packet.end(), pdu.begin(), pdu.end());

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

void write_single_coil::decode_passed(const packet_t& packet) {
  try {
    if (packet.size() != request_->response_size()) {
      throw ex::bad_data();
    }

    packet_t::size_type address_idx = header_length + 1;

    std::uint16_t address, value;
    struc::unpack(fmt::format(">{}", format), packet.data() + address_idx,
                  address, value);

    if (request_->address()() != address) {
      logger::get()->debug("ResponseWriteSingleCoil: Address mismatch");
      throw ex::bad_data();
    }

    if (utilities::to_underlying(request_->value()) != value &&
        check_bits_value(value)) {
      logger::get()->debug("ResponseWriteSingleCoil: Value mismatch");
      throw ex::bad_data();
    }

    value_ = static_cast<value::bits>(value);
  } catch (...) {
    throw ex::bad_data();
  }
}

std::ostream& write_single_coil::dump(std::ostream& os) const {
  fmt::print(
      os,
      "ResponseWriteSingleCoil(header[transaction={:#04x}, protocol={:#04x}, "
      "unit={:#04x}], pdu[function={:#04x}, address={:#04x}, "
      "value={:#04x}])",
      transaction_, protocol, unit_, function_code_, request_->address()(),
      utilities::to_underlying(request_->value()));
  return os;
}
}  // namespace response
}  // namespace modbus
