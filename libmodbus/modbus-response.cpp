#include "modbus.hpp"

#include "modbus-response.hpp"

#include "modbus-logger.hpp"

#include "modbus-types.hpp"
#include "modbus-utilities.hpp"

namespace modbus {
namespace internal {
response::response() noexcept {}

response::response(constants::function_code function,
                   table*                   data_table) noexcept
    : adu{function}, data_table_{data_table} {}

bool response::initial_check(const packet_t& packet) {
  return packet.size() > header_length;
}

stage response::check_stage(const packet_t& packet) {
  // 1. check packet size (at least we got header and the function code)
  if (!initial_check(packet)) {
    // bad packet
    return stage::bad;
  }

  // 2. unpack packet and initialize header metadata
  decode_header(packet);

  // 3. check expected function is valid or not
  if (!check_function(function())) {
    // bad packet or defined class
    return stage::bad;
  }

  auto expected_function = utilities::to_underlying(function());

  // 4. check expected function code equals with function code from packet
  if ((expected_function != function_code_) &&
      !check_function(function_code_)) {
    std::uint8_t diff = function_code_ - 0x80;

    // 5. if expected function equals packet's function_code - 0x80
    //    exception is occured
    if (expected_function == diff) {
      return stage::error;
    }

    // bad packet
    return stage::bad;
  }

  // 6. all tests passed, return the "right" response
  return stage::passed;
}

void response::decode(const packet_t& packet) {
  try {
    switch (check_stage(packet)) {
      case internal::stage::bad:
        throw ex::bad_data();
      case internal::stage::error: {
        // decode the packet
        auto exc = packet.at(header_length + 1);
        throw_exception(static_cast<constants::exception_code>(exc), function(),
                        header());
      } break;
      default:
        decode_passed(packet);
        break;
    }
  } catch (const std::out_of_range&) {
    // anything happens, such as packet is malformed
    throw ex::bad_data();
  }
}
}  // namespace internal

namespace response {
error::error() noexcept {}

packet_t error::encode() {
  calc_length(1);
  packet_t packet = header_packet();
  packet.pop_back();
  packet.reserve(header_length + 1 + 1);
  packet_t pdu = struc::pack(fmt::format(">{}", format),
                             utilities::to_underlying(function()) + 0x80,
                             utilities::to_underlying(ec_));
  packet.insert(packet.end(), pdu.begin(), pdu.end());

  if (packet.size() != calc_adu_length(1)) {
    throw ex::bad_data();
  }

  return packet;
}

void error::decode(const packet_t& packet) {
  decode_header(packet);

  std::uint8_t ec;
  struc::unpack(fmt::format(">{}", format), packet.data() + header_length + 1,
                ec);

  if (!check_exception(ec)) {
    throw ex::bad_exception();
  }

  ec_ = static_cast<constants::exception_code>(ec);
}
}  // namespace response
}  // namespace modbus
