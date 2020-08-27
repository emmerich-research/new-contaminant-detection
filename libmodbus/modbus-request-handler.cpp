#include "modbus.hpp"

#include "modbus-request-handler.hpp"

#include <utility>

#include <fmt/core.h>
#include <fmt/ostream.h>

#include "modbus-constants.hpp"
#include "modbus-exception.hpp"
#include "modbus-utilities.hpp"

#include "modbus-adu.hpp"

#include "modbus-bit-read.hpp"
#include "modbus-response.hpp"

namespace modbus {
packet_t request_handler::handle(table&                  data_table,
                                 const std::string_view& packet) {
  packet_t pack{packet.begin(), packet.end()};
  return handle(data_table, pack);
}

packet_t request_handler::handle(table& data_table, const packet_t& packet) {
  constexpr auto header_length = internal::adu::header_length;

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
        auto&& res = req.execute(data_table);
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

      case constants::function_code::read_write_multiple_registers: {
      } break;

      default:
        break;
        // throw ex::illegal_function{};
    }
  } catch (const ex::specification_error& exc) {
    logger::get()->error("Modbus exception occured: {}", exc.what());
    response::error response(exc);
    auto            resp = response.encode();
    logger::get()->error("Exception packet: {}", utilities::packet_str(resp));
    return resp;
  } catch (const ex::base_error& exc) {
    logger::get()->error("Internal exception occured: {}", exc.what());
  } catch (const std::out_of_range& exc) {
    logger::get()->error("Out of range exception occured: {}", exc.what());
  } catch (const std::exception& exc) {
    logger::get()->error("Unintended exception occured {}", exc.what());
  }

  return {};
}
}  // namespace modbus
