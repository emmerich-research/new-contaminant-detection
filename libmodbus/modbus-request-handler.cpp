#include "modbus.hpp"

#include "modbus-request-handler.hpp"

#include <fmt/core.h>
#include <fmt/ostream.h>

#include "modbus-constants.hpp"
#include "modbus-exception.hpp"

#include "modbus-adu.hpp"

#include "modbus-bit-read.hpp"

namespace modbus {
request_handler::request_handler(table* data_table) : data_table_{data_table} {}

internal::packet_t request_handler::handle(const std::string_view& packet) {
  internal::packet_t pack{packet.begin(), packet.end()};
  return handle(pack);
}

internal::packet_t request_handler::handle(const internal::packet_t& packet) {
  constexpr auto header_length =
      internal::adu<constants::function_code::read_coils>::header_length;

  try {
    if (packet.size() <= header_length) {
      throw ex::bad_data_size();
    }

    constants::function_code function =
        static_cast<constants::function_code>(packet.at(header_length));

    switch (function) {
      case constants::function_code::read_coils: {
        request::read_coils req;
        req.decode(packet);
        auto&& res = req.execute(*data_table_);
        return res->encode();
      } break;

      case constants::function_code::read_discrete_inputs: {
      } break;

      case constants::function_code::read_holding_registers: {
      } break;

      case constants::function_code::read_input_registers: {
      } break;

      case constants::function_code::write_single_coil: {
      } break;

      case constants::function_code::write_single_register: {
      } break;

      case constants::function_code::write_multiple_coils: {
      } break;

      case constants::function_code::write_multiple_registers: {
      } break;

      case constants::function_code::mask_write_register: {
      } break;

      case constants::function_code::read_write_multiple_registers: {
      } break;

      default:
        throw ex::illegal_function();
    }
  } catch (const ex::bad_data& exc) {
    return {1, 2, 3, 4};
  } catch (const ex::bad_data_size& exc) {
    return {1, 2, 3, 4};
  } catch (const ex::illegal_function& exc) {
    return {1, 2, 3, 4};
  } catch (const ex::illegal_data_address exc) {
    return {1, 2, 3, 4};
  } catch (const ex::illegal_data_value exc) {
    return {1, 2, 3, 4};
  }
}
}  // namespace modbus
