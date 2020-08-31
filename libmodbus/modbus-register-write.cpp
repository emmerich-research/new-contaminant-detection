#include "modbus.hpp"

#include "modbus-register-write.hpp"

#include <algorithm>
#include <exception>
#include <iterator>

#include <struc.hpp>

#include "modbus-exception.hpp"
#include "modbus-logger.hpp"
#include "modbus-operation.hpp"
#include "modbus-utilities.hpp"

namespace modbus {
namespace request {
write_single_register::write_single_register(const address_t&   address,
                                             const reg_value_t& value) noexcept
    : internal::request{constants::function_code::write_single_register},
      address_{address},
      value_{value} {}

typename packet_t::size_type write_single_register::response_size() const {
  return calc_adu_length(data_length);
}

packet_t write_single_register::encode() {
  if (!address_.validate()) {
    throw ex::bad_data();
  }

  calc_length(data_length);
  packet_t packet = header_packet();
  packet.reserve(header_length + 1 + data_length);
  packet_t pdu = struc::pack(fmt::format(">{}", format), address_(), value_());
  packet.insert(packet.end(), pdu.begin(), pdu.end());
  return packet;
}

void write_single_register::decode(const packet_t& packet) {
  try {
    if (packet.at(header_length) != utilities::to_underlying(function())) {
      throw ex::bad_data();
    }

    decode_header(packet);

    struc::unpack(fmt::format(">{}", format), packet.data() + header_length + 1,
                  address_.ref(), value_.ref());

    // if (!(0x0000 <= value && value <= 0xFFFF)) {
    // value_ = value;
    //} else {
    // throw ex::bad_data();
    //}
  } catch (...) {
    throw ex::server_device_failure(function(), header());
  }
}

typename internal::response::pointer write_single_register::execute(
    table* data_table) {
  if (!value_.validate()) {
    throw ex::illegal_data_value(function(), header());
  }

  if (!data_table->holding_registers().validate(address_)) {
    throw ex::illegal_data_address(function(), header());
  }

  return response::write_single_register::create(this, data_table);
}

std::ostream& write_single_register::dump(std::ostream& os) const {
  fmt::print(os,
             "RequestWriteSingleRegister(header[transaction={:#04x}, "
             "protocol={:#04x}, "
             "unit={:#04x}], pdu[function={:#04x}, address={:#04x}, "
             "value={:#04x}])",
             transaction_, protocol, unit_, function_code_, address()(),
             value_);
  return os;
}

write_multiple_registers::write_multiple_registers(
    const address_t&                                   address,
    const write_num_regs_t&                            count,
    std::initializer_list<block::registers::data_type> values) noexcept
    : internal::request{constants::function_code::write_multiple_registers},
      address_{address},
      count_{count},
      values_(values) {
  byte_count_ = byte_count();
}

packet_t write_multiple_registers::encode() {
  if (!address_.validate() || !count_.validate()) {
    throw ex::bad_data();
  }

  calc_length(data_length());
  packet_t packet = header_packet();
  packet.reserve(header_length + data_length());
  packet_t pdu = struc::pack(fmt::format(">{}", format), address_(), count_(),
                             byte_count());
  packet.insert(packet.end(), pdu.begin(), pdu.end());

  for (const auto& value : values_) {
    auto regs = struc::pack(">H", value);
    packet.insert(packet.end(), regs.begin(), regs.end());
  }

  if (packet.size() != (data_length() + header_length + 1)) {
    throw ex::bad_data();
  }

  return packet;
}

void write_multiple_registers::decode(const packet_t& packet) {
  try {
    if (packet.at(header_length) != utilities::to_underlying(function())) {
      throw ex::bad_data();
    }

    decode_header(packet);

    packet_t::size_type values_idx = header_length + 1 + 5;
    std::uint8_t        byte_count_recv;
    struc::unpack(fmt::format(">{}", format), packet.data() + header_length + 1,
                  address_.ref(), count_.ref(), byte_count_recv);
    byte_count_ = byte_count_recv;

    block::registers::container_type buffer;

    for (int idx = 0; idx < byte_count(); idx += 2) {
      std::uint16_t value;
      struc::unpack(">H", packet.data() + values_idx + idx, value);
      buffer.push_back(value);
    }

    buffer.resize(count_());
    values_.swap(buffer);
  } catch (...) {
    throw ex::server_device_failure(function(), header());
  }
}

typename internal::response::pointer write_multiple_registers::execute(
    table* data_table) {
  if (!count_.validate()) {
    throw ex::illegal_data_value(function(), header());
  }

  if (!data_table->holding_registers().validate(address_, count_)) {
    throw ex::illegal_data_address(function(), header());
  }

  return response::write_multiple_registers::create(this, data_table);
}

typename packet_t::size_type write_multiple_registers::response_size() const {
  return calc_adu_length(4);
}

std::uint8_t write_multiple_registers::byte_count() const {
  return static_cast<std::uint8_t>(count_() * 2);
}

std::ostream& write_multiple_registers::dump(std::ostream& os) const {
  fmt::print(
      os,
      "RequestWriteMultipleRegisters(header[transaction={:#04x}, "
      "protocol={:#04x}, "
      "unit={:#04x}], pdu[function={:#04x}, address={:#04x}, quantity={:#04x}, "
      "bytes_count={:#04x}, values_size={}])",
      transaction_, protocol, unit_, function_code_, address(), count(),
      byte_count(), values_.size());
  return os;
}
}  // namespace request

namespace response {
write_single_register::write_single_register(
    const request::write_single_register* request,
    table*                                data_table) noexcept
    : internal::response{request->function(), request->header(), data_table},
      request_{request} {
  initialize({request_->transaction(), request_->unit()});
}

packet_t write_single_register::encode() {
  try {
    calc_length(data_length);
    packet_t packet = header_packet();
    packet.reserve(request_->response_size());
    packet_t pdu = struc::pack(fmt::format(">{}", format),
                               request_->address()(), request_->value()());
    packet.insert(packet.end(), pdu.begin(), pdu.end());

    if (!request_->check_response_packet(packet)) {
      throw ex::server_device_failure(function(), header());
    }

    data_table()->holding_registers().set(request_->address(),
                                          request_->value()());
    address_ = request_->address();
    value_ = request_->value();

    return packet;
  } catch (const std::out_of_range&) {
    throw ex::illegal_data_address(function(), header());
  } catch (...) {
    throw ex::server_device_failure(function(), header());
  }
}

void write_single_register::decode_passed(const packet_t& packet) {
  try {
    if (packet.size() != request_->response_size()) {
      throw ex::bad_data();
    }

    packet_t::size_type address_idx = header_length + 1;

    std::uint16_t address, value;
    struc::unpack(fmt::format(">{}", format), packet.data() + address_idx,
                  address, value);

    if (request_->address()() != address) {
      logger::debug("ResponseWriteSingleRegister: Address mismatch");
      throw ex::bad_data();
    }

    if (request_->value()() != value) {
      logger::debug("ResponseWriteSingleRegister: Value mismatch");
      throw ex::bad_data();
    }

    address_ = request_->address();
    value_ = request_->value();
  } catch (...) {
    throw ex::bad_data();
  }
}

std::ostream& write_single_register::dump(std::ostream& os) const {
  fmt::print(os,
             "ResponseWriteSingleRegister(header[transaction={:#04x}, "
             "protocol={:#04x}, "
             "unit={:#04x}], pdu[function={:#04x}, address={:#04x}, "
             "value={:#04x}])",
             transaction_, protocol, unit_, function_code_, address(), value_);
  return os;
}

write_multiple_registers::write_multiple_registers(
    const request::write_multiple_registers* request,
    table*                                   data_table) noexcept
    : internal::response{request->function(), request->header(), data_table},
      request_{request} {
  initialize({request_->transaction(), request_->unit()});
}

packet_t write_multiple_registers::encode() {
  try {
    calc_length(data_length);
    packet_t packet = header_packet();
    packet.reserve(request_->response_size());
    packet_t pdu = struc::pack(fmt::format(">{}", format),
                               request_->address()(), request_->count()());
    packet.insert(packet.end(), pdu.begin(), pdu.end());
    data_table()->holding_registers().set(request_->address(),
                                          request_->values());
    return packet;
  } catch (const std::out_of_range&) {
    throw ex::illegal_data_address(function(), header());
  } catch (...) {
    throw ex::server_device_failure(function(), header());
  }
}

void write_multiple_registers::decode_passed(const packet_t& packet) {
  try {
    if (packet.size() != request_->response_size()) {
      throw ex::bad_data();
    }

    packet_t::size_type address_idx = header_length + 1;

    struc::unpack(fmt::format(">{}", format), packet.data() + address_idx,
                  address_.ref(), count_.ref());

    if (request_->address() != address_) {
      logger::debug("ResponseWriteMultipleRegisters: Address mismatch");
      throw ex::bad_data();
    }

    if (request_->count() != count_) {
      logger::debug("ResponseWriteMultipleRegisters: Count mismatch");
      throw ex::bad_data();
    }
  } catch (...) {
    throw ex::bad_data();
  }
}

std::ostream& write_multiple_registers::dump(std::ostream& os) const {
  fmt::print(os,
             "ResponseWriteMultipleRegisters(header[transaction={:#04x}, "
             "protocol={:#04x}, "
             "unit={:#04x}], pdu[function={:#04x}, address={:#04x}])",
             transaction_, protocol, unit_, function_code_, address());
  return os;
}
}  // namespace response
}  // namespace modbus
