#include "networking.hpp"

#include "modbus.hpp"

#include <libutil/util.hpp>

NAMESPACE_BEGIN

namespace networking {
// const unsigned int  Modbus::MAX_PDU_LENGTH = 253;
// const unsigned int  Modbus::MAX_ADU_LENGTH = 260;
// const unsigned int  Modbus::MAX_MESSAGE_LENGTH = MAX_ADU_LENGTH;
// const unsigned int  Modbus::MIN_REQ_LENGTH = 12;
// const std::uint16_t Modbus::MAX_READ_BITS = 0x07D0;
// const std::uint16_t Modbus::MAX_WRITE_BITS = 0x07B0;
// const std::uint16_t Modbus::MAX_READ_REGISTERS = 0x007D;
// const std::uint16_t Modbus::MAX_WRITE_REGISTERS = 0x007B;
// const std::uint16_t Modbus::MAX_RW_READ_REGISTERS = 0x007D;
// const std::uint16_t Modbus::MAX_RW_WRITE_REGISTERS = 0x0079;

Modbus::Modbus(const unsigned int& header_length)
    : header_length_{header_length}, transaction_id_{0}, slave_id_{0xFF} {
  DEBUG_ONLY(obj_name_ = "Modbus");
  LOG_DEBUG("Initializing Modbus with header length {}", header_length);
}

Modbus::~Modbus() {}

void Modbus::slave_id(const std::uint8_t& slave_id) {
  slave_id_ = slave_id;
}

bool Modbus::check_quantity(const std::uint16_t& quantity,
                            const std::uint16_t& max_quantity) {
  return quantity >= 1 && quantity <= max_quantity;
}

bool Modbus::check_exception(const modbus::exception& exception) {
  return check_exception(util::to_underlying(exception));
}

bool Modbus::check_exception(const std::uint8_t& exception) {
  return exception > util::to_underlying(modbus::exception::min_exception) &&
         exception < util::to_underlying(modbus::exception::max_exception) &&
         exception != util::to_underlying(modbus::exception::undef);
}

const char* Modbus::exception_message(const modbus::exception& exception) {
  switch (exception) {
      /** Modbus exception */
    case modbus::exception::illegal_function:
      return "Illegal function";
    case modbus::exception::illegal_data_address:
      return "Illegal data address";
    case modbus::exception::illegal_data_value:
      return "Illegal data value";
    case modbus::exception::server_device_failure:
      return "Master or slave failure";
    case modbus::exception::acknowledge:
      return "Acknowledge";
    case modbus::exception::server_device_busy:
      return "Master or slave is busy";
    case modbus::exception::negative_acknowledge:
      return "Negative acknowledge";
    case modbus::exception::memory_parity_error:
      return "Memory parity error";
    case modbus::exception::gateway_path_unavailable:
      return "Gateway path unavailable";
    case modbus::exception::gateway_target_device_failed_to_respond:
      return "Target device failed to respond";
      /** Internal exception */
    case modbus::exception::bad_data:
      return "Invalid data";
    case modbus::exception::bad_data_size:
      return "Invalid size of data";
    case modbus::exception::connection_problem:
      return "Connection problem";
    case modbus::exception::bad_exception:
      return "Invalid Modbus exception code";
    default:
      return "Not an exception, should not be called";
  }
}

bool Modbus::check_function(const modbus::function& function) {
  return check_function(util::to_underlying(function));
}

bool Modbus::check_function(const std::uint8_t& function) {
  switch (function) {
    case util::to_underlying(modbus::function::read_coils):
    case util::to_underlying(modbus::function::read_discrete_inputs):
    case util::to_underlying(modbus::function::read_holding_registers):
    case util::to_underlying(modbus::function::write_single_coil):
    case util::to_underlying(modbus::function::write_single_register):
    case util::to_underlying(modbus::function::read_exception_status):
    case util::to_underlying(modbus::function::diagnostics):
    case util::to_underlying(modbus::function::write_multiple_coils):
    case util::to_underlying(modbus::function::read_file_record):
    case util::to_underlying(modbus::function::mask_write_register):
    case util::to_underlying(modbus::function::read_write_multiple_registers):
    case util::to_underlying(modbus::function::read_fifo_queue):
    case util::to_underlying(
        modbus::function::encapsulated_interface_transport):
      return true;
    default:
      return false;
  }
}

unsigned int Modbus::calculate_length_from_request(
    const const std::uint8_t* request) const {
  const auto offset = header_length();

  unsigned int length;

  switch (request[offset]) {
    case util::to_underlying(modbus::function::read_coils):
    case util::to_underlying(modbus::function::read_discrete_inputs): {
      std::uint16_t num_of_bytes = uint8_to_uint16(request, offset + 3);
      length = 2 + (num_of_bytes / 8) + ((num_of_bytes % 8) ? 1 : 0);
    } break;

    case util::to_underlying(modbus::function::read_write_multiple_registers):
    case util::to_underlying(modbus::function::read_holding_registers):
    case util::to_underlying(modbus::function::read_input_registers):
      length = 2 + 2 * uint8_to_uint16(request, offset + 3);
      break;

    case util::to_underlying(modbus::function::write_single_coil):
    case util::to_underlying(modbus::function::write_single_register):
    case util::to_underlying(modbus::function::write_multiple_coils):
    case util::to_underlying(modbus::function::write_multiple_registers):
      length = 5;
      break;

    case util::to_underlying(modbus::function::mask_write_register):
      length = 7;
      break;

    default:
      length = 0;
      break;
  }

  return offset + length;
}

bool Modbus::check_confirmation(const std::uint8_t* request,
                                const std::uint8_t* response) const {
  // check transaction id
  if ((request[0] != response[0]) || (request[1] != response[1])) {
    LOG_INFO("Invalid transaction id, should be 0x{} but got 0x{}",
             uint8_to_uint16(request), uint8_to_uint16(response));
    return false;
  }

  // check protocol, TCP should be 0x00 for address 2 and 3
  if ((response[2] != 0) && (response[3] != 0)) {
    LOG_INFO("Invalid protocol id, should be 0x00 but got 0x{}",
             uint8_to_uint16(response, 2));
    return false;
  }

  return true;
}

modbus::exception Modbus::check_exception(
    const std::uint8_t* request,
    const std::uint8_t* response,
    const std::size_t&  response_length) const {
  const std::uint8_t request_function = request[header_length()];
  const std::uint8_t response_function = response[header_length()];

  const auto check_exception_length = [](const unsigned int&      offset,
                                         const const std::size_t& res_length) {
    /** Read Modbus_Application_Protocol_Specification_V1_1b3.pdf (chapter 7
     * page 47), Modbus exception will only add 2 bytes (function and exception
     * code)
     */
    return res_length == (offset + 2);
  };

  if (response_function >= 0x80 &&
      check_exception_length(header_length(), response_length) &&
      (request_function == (response_function - 0x80))) {
    // exception is happening here
    const std::uint8_t exception_code = response[header_length() + 1];

    if (check_exception(exception_code)) {
      return static_cast<modbus::exception>(exception_code);
    } else {
      return modbus::exception::bad_exception;
    }
  }

  return modbus::exception::no_exception;
}

bool Modbus::check_length(const std::uint8_t* request,
                          const std::uint8_t* response,
                          const std::size_t&  response_length) const {
  massert(response[header_length()] < 0x80,
          "function should be less than 0x80");
  const std::uint8_t function = response[header_length()];
  const unsigned int expected_length = calculate_length_from_request(request);
  const auto         offset = header_length();

  if (response_length == expected_length) {
    // check if function is valid
    if (check_function(function)) {
      LOG_INFO("Response function is invalid");
      return false;
    }

    // check whether response function is same with request function
    if (request[header_length()] != function) {
      LOG_INFO("Request function is not equal with response function");
      return false;
    }

    const modbus::function e_function = static_cast<modbus::function>(function);

    std::uint8_t req_num_of_bytes;
    std::uint8_t res_num_of_bytes;

    switch (e_function) {
      case modbus::function::read_coils:
      case modbus::function::read_discrete_inputs: {
        req_num_of_bytes = (request[offset + 3] << 8) + request[offset + 4];
        req_num_of_bytes =
            (req_num_of_bytes / 8) + ((req_num_of_bytes % 8) ? 1 : 0);
        res_num_of_bytes = response[offset + 1];
      } break;

      case modbus::function::read_write_multiple_registers:
      case modbus::function::read_holding_registers:
      case modbus::function::read_input_registers:
        req_num_of_bytes = (request[offset + 3] << 8) + request[offset + 4];
        res_num_of_bytes = response[offset + 1] / 2;
        break;

      case modbus::function::write_multiple_coils:
      case modbus::function::write_multiple_registers:
        req_num_of_bytes = (request[offset + 3] << 8) + request[offset + 4];
        res_num_of_bytes = (response[offset + 3] << 8) | response[offset + 4];
        break;

      // other than above should be equal
      case modbus::function::write_single_coil:
      case modbus::function::write_single_register:
        req_num_of_bytes = res_num_of_bytes = 1;
        break;

      default:
        req_num_of_bytes = res_num_of_bytes = 0;
        break;
    }

    if (req_num_of_bytes != res_num_of_bytes) {
      LOG_INFO("Quantity of request is not equal with response");
      return false;
    }

    // no error
    return true;
  }

  LOG_INFO("Expected length from request and response length is mismatch");
  return false;
}

void Modbus::uint16_to_uint8(std::uint8_t*        buffer,
                             const std::uint16_t& value,
                             const std::uint16_t& start_addr) {
  massert(start_addr < MAX_MESSAGE_LENGTH, "sanity");
  buffer[start_addr] = value >> 8;
  buffer[start_addr + 1] = value & 0x00FF;
}

void Modbus::uint8_to_uint16(const std::uint8_t*  buffer,
                             std::uint16_t&       value,
                             const std::uint16_t& start_addr) {
  massert(start_addr < MAX_MESSAGE_LENGTH, "sanity");
  value = (buffer[start_addr] << 8) | buffer[start_addr + 1];
}

std::uint16_t Modbus::uint8_to_uint16(const std::uint8_t*  buffer,
                                      const std::uint16_t& start_addr) {
  massert(start_addr < MAX_MESSAGE_LENGTH, "sanity");
  return (buffer[start_addr] << 8) | buffer[start_addr + 1];
}

void Modbus::read_bits(const std::uint16_t& address,
                       const std::uint16_t& quantity,
                       std::uint8_t*        buffer) {
  if (!check_quantity(quantity, MAX_READ_BITS)) {
    return;
  }

  std::uint8_t req[MIN_REQ_LENGTH];
  std::uint8_t res[MAX_MESSAGE_LENGTH];

  unsigned int req_length =
      build_request(req, modbus::function::read_coils, address, quantity);

  // int req_ans_size = send_message(req, req_length);

  // if (req_ans_size > 0) {
  // }
}

void Modbus::read_input_bits(const std::uint16_t& address,
                             const std::uint16_t& quantity,
                             std::uint8_t*        buffer) {}

void Modbus::read_registers(const std::uint16_t& address,
                            const std::uint16_t& quantity,
                            std::uint16_t*       buffer) {}

void Modbus::read_input_registers(const std::uint16_t& address,
                                  const std::uint16_t& quantity,
                                  std::uint16_t*       buffer) {}

void Modbus::write_bit(const std::uint16_t& address, const uint8_t& value) {}

void Modbus::write_bits(const std::uint16_t& address,
                        const std::uint16_t& quantity,
                        const std::uint8_t*  value) {}

void Modbus::write_register(const std::uint16_t& address,
                            const std::uint16_t& value) {}

void Modbus::write_registers(const std::uint16_t& address,
                             const std::uint16_t& quantity,
                             const std::uint16_t* value) {}
}  // namespace networking

NAMESPACE_END
