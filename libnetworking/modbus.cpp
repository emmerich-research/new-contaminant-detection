#include "networking.hpp"

#include "modbus.hpp"

#include <utility>

#include <libutil/util.hpp>

NAMESPACE_BEGIN

namespace networking {
const Modbus::Timeout Modbus::MaxTimeout = modbus::time::minutes(1);

Modbus::Modbus(const unsigned int&    header_length,
               const Modbus::Timeout& connect_timeout,
               const Modbus::Timeout& request_timeout,
               const Modbus::Timeout& response_timeout)
    : header_length_{header_length},
      transaction_id_{0},
      slave_id_{0xFF},
      connect_timeout_{connect_timeout},
      request_timeout_{request_timeout},
      response_timeout_{response_timeout} {
  DEBUG_ONLY(obj_name_ = "Modbus");
  LOG_DEBUG("Initializing Modbus with header length {}", header_length);
  response_callback_ = []([[maybe_unused]] const auto& response) {};
  error_callback_ = []([[maybe_unused]] const auto& error) {};
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
    case util::to_underlying(modbus::function::read_input_registers):
    case util::to_underlying(modbus::function::write_single_coil):
    case util::to_underlying(modbus::function::write_single_register):
    case util::to_underlying(modbus::function::read_exception_status):
    case util::to_underlying(modbus::function::diagnostics):
    case util::to_underlying(modbus::function::write_multiple_coils):
    case util::to_underlying(modbus::function::write_multiple_registers):
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
    const Buffer& request) const {
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

unsigned int Modbus::calculate_next_response_length_after(
    const modbus::phase& phase,
    const Buffer&        response) const {
  const modbus::function function =
      static_cast<modbus::function>(response[header_length()]);

  if (phase == modbus::phase::function) {
    switch (function) {
      case modbus::function::write_single_coil:
      case modbus::function::write_single_register:
      case modbus::function::write_multiple_coils:
      case modbus::function::write_multiple_registers:
        return 4;

      case modbus::function::mask_write_register:
        return 6;

      default:
        return 1;
    }
  } else {
    switch (function) {
      case modbus::function::read_coils:
      case modbus::function::read_discrete_inputs:
      case modbus::function::read_holding_registers:
      case modbus::function::read_input_registers:
      case modbus::function::read_write_multiple_registers:
        return response[header_length() + 1];

      default:
        return 0;
    }
  }
}

bool Modbus::check_confirmation(const Buffer& request,
                                const Buffer& response) const {
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
    const Buffer&      request,
    const Buffer&      response,
    const std::size_t& response_length) const {
  const std::uint8_t request_function = request[header_length()];
  const std::uint8_t response_function = response[header_length()];

  const auto check_exception_length = [](const unsigned int& offset,
                                         const std::size_t&  res_length) {
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

std::optional<unsigned int> Modbus::check_length(
    const Buffer&      request,
    const Buffer&      response,
    const std::size_t& response_length) const {
  massert(response[header_length()] < 0x80,
          "function should be less than 0x80");
  const std::uint8_t function = response[header_length()];
  const unsigned int expected_length = calculate_length_from_request(request);
  const auto         offset = header_length();

  if (response_length == expected_length) {
    // check if function is valid
    if (!check_function(function)) {
      LOG_ERROR("Response function is invalid");
      return {};
    }

    // check whether response function is same with request function
    if (request[header_length()] != function) {
      LOG_ERROR("Request function is not equal with response function");
      return {};
    }

    const modbus::function e_function = static_cast<modbus::function>(function);

    unsigned int req_num_of_bytes;
    unsigned int res_num_of_bytes;

    switch (e_function) {
      case modbus::function::read_coils:
      case modbus::function::read_discrete_inputs: {
        req_num_of_bytes = static_cast<unsigned int>(request[offset + 3] << 8) +
                           request[offset + 4];
        req_num_of_bytes =
            (req_num_of_bytes / 8) + ((req_num_of_bytes % 8) ? 1 : 0);
        res_num_of_bytes = response[offset + 1];
      } break;

      case modbus::function::read_write_multiple_registers:
      case modbus::function::read_holding_registers:
      case modbus::function::read_input_registers:
        req_num_of_bytes = static_cast<unsigned int>(request[offset + 3] << 8) +
                           request[offset + 4];
        res_num_of_bytes = response[offset + 1] / 2;
        break;

      case modbus::function::write_multiple_coils:
      case modbus::function::write_multiple_registers:
        req_num_of_bytes = static_cast<unsigned int>(request[offset + 3] << 8) +
                           request[offset + 4];
        res_num_of_bytes =
            static_cast<unsigned int>(uint8_to_uint16(response, offset + 3));
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
      LOG_ERROR("Quantity of request is not equal with response");
      return {};
    }

    // no error, returning num of bytes
    return req_num_of_bytes;
  }

  LOG_ERROR("Expected length from request and response length is mismatch");
  return {};
}

void Modbus::uint16_to_uint8(Buffer&              buffer,
                             const std::uint16_t& value,
                             const unsigned int&  start_addr) {
  massert(start_addr < MAX_MESSAGE_LENGTH, "sanity");
  buffer[start_addr] = value >> 8;
  buffer[start_addr + 1] = value & 0x00FF;
}

std::uint16_t Modbus::uint8_to_uint16(const Buffer&       buffer,
                                      const unsigned int& start_addr) {
  massert(start_addr < MAX_MESSAGE_LENGTH, "sanity");
  return static_cast<std::uint16_t>(buffer[start_addr] << 8) |
         buffer[start_addr + 1];
}

Modbus::Response Modbus::process_read_bits(const modbus::function& function,
                                           const std::uint16_t&    address,
                                           const std::uint16_t&    quantity,
                                           Buffer8&                buffer) {
  massert(check_quantity(quantity, MAX_READ_BITS), "sanity");

  Buffer request;

  unsigned int request_length =
      build_request(request, function, address, quantity);

  auto&& response = send(request, request_length);

  if (error(response)) {
    error_callback()(get_error(response));
  } else {
    auto&        res = get_response(response);
    unsigned int temp;
    unsigned int bit;
    unsigned int pos = 0;
    unsigned int offset = header_length() + 2;
    unsigned int offset_end = offset + quantity;

    res.data = Buffer8{};
    auto&& data = std::get<Buffer8>(res.data);

    for (size_t i = offset; i < offset_end; i++) {
      temp = res.response[i];

      for (bit = 0x01; (bit & 0xff) && (pos < quantity);) {
        data[pos++] = (temp & bit) ? 1 : 0;
        bit = bit << 1;
      }
    }

    buffer = data;
    response_callback()(res);
  }

  return std::move(response);
}

Modbus::Response Modbus::read_bits(const std::uint16_t& address,
                                   const std::uint16_t& quantity,
                                   Buffer&              buffer) {
  if (!check_quantity(quantity, MAX_READ_BITS)) {
    LOG_ERROR("Too many bits requested");
    return ModbusError{
        modbus::exception::bad_data_size,
        Modbus::exception_message(modbus::exception::bad_data_size),
        /** internal */ true};
  }

  LOG_INFO("Read bits with address={} and quantity={}", address, quantity);

  return process_read_bits(modbus::function::read_coils, address, quantity,
                           buffer);
}

Modbus::Response Modbus::read_input_bits(const std::uint16_t& address,
                                         const std::uint16_t& quantity,
                                         Buffer8&             buffer) {
  if (!check_quantity(quantity, MAX_READ_BITS)) {
    LOG_ERROR("Too many input bits requested");
    return ModbusError{
        modbus::exception::bad_data_size,
        Modbus::exception_message(modbus::exception::bad_data_size),
        /** internal */ true};
  }

  LOG_INFO("Read input bits with address={} and quantity={}", address,
           quantity);

  return process_read_bits(modbus::function::read_discrete_inputs, address,
                           quantity, buffer);
}

Modbus::Response Modbus::process_read_registers(
    const modbus::function& function,
    const std::uint16_t&    address,
    const std::uint16_t&    quantity,
    Buffer16&               buffer) {
  massert(check_quantity(quantity, MAX_READ_REGISTERS), "sanity");

  Buffer request;

  unsigned int request_length =
      build_request(request, function, address, quantity);

  auto&& response = send(request, request_length);

  if (error(response)) {
    error_callback()(get_error(response));
  } else {
    auto& res = get_response(response);
    res.data = Buffer16{};
    auto&& data = std::get<Buffer16>(res.data);

    unsigned int offset = header_length();

    for (size_t i = 0; i < res.num_of_bytes; ++i) {
      data[i] = uint8_to_uint16(res.response,
                                offset + 2 + static_cast<unsigned int>(i << 1));
    }

    buffer = data;
    response_callback()(res);
  }

  return std::move(response);
}

Modbus::Response Modbus::read_registers(const std::uint16_t& address,
                                        const std::uint16_t& quantity,
                                        Buffer16&            buffer) {
  if (!check_quantity(quantity, MAX_READ_REGISTERS)) {
    LOG_ERROR("Too many registers requested");
    return ModbusError{
        modbus::exception::bad_data_size,
        Modbus::exception_message(modbus::exception::bad_data_size),
        /** internal */ true};
  }

  LOG_INFO("Read registers with address={} and quantity={}", address, quantity);

  return process_read_registers(modbus::function::read_holding_registers,
                                address, quantity, buffer);
}

Modbus::Response Modbus::read_input_registers(const std::uint16_t& address,
                                              const std::uint16_t& quantity,
                                              Buffer16&            buffer) {
  if (!check_quantity(quantity, MAX_READ_REGISTERS)) {
    LOG_ERROR("Too many input registers requested");
    return ModbusError{
        modbus::exception::bad_data_size,
        Modbus::exception_message(modbus::exception::bad_data_size),
        /** internal */ true};
  }

  LOG_INFO("Read input registers with address={} and quantity={}", address,
           quantity);

  return process_read_registers(modbus::function::read_input_registers, address,
                                quantity, buffer);
}

Modbus::Response Modbus::process_write_single(const modbus::function& function,
                                              const std::uint16_t&    address,
                                              const uint16_t&         value) {
  Buffer request;

  unsigned int request_length =
      build_request(request, function, address, value);

  auto&& response = send(request, request_length);

  if (error(response)) {
    error_callback()(get_error(response));
  } else {
    response_callback()(get_response(response));
  }

  return std::move(response);
}

Modbus::Response Modbus::write_bit(const std::uint16_t& address,
                                   const bool&          value) {
  LOG_INFO("Write single bit/coil with address={} and value={}", address,
           value);
  return process_write_single(modbus::function::write_single_coil, address,
                              value ? 0xFF00 : 0x0);
}

Modbus::Response Modbus::write_register(const std::uint16_t& address,
                                        const std::uint16_t& value) {
  LOG_INFO("Write single register with address={} and value={}", address,
           value);
  return process_write_single(modbus::function::write_single_register, address,
                              value);
}

Modbus::Response Modbus::write_bits(const std::uint16_t& address,
                                    const std::uint16_t& quantity,
                                    const std::uint8_t*  value) {
  if (!check_quantity(quantity, MAX_WRITE_BITS)) {
    LOG_ERROR("Too many bits that need to write");
    return ModbusError{
        modbus::exception::bad_data_size,
        Modbus::exception_message(modbus::exception::bad_data_size),
        /** internal */ true};
  }

  LOG_INFO("Write bits with address={}", address);

  Buffer        request;
  std::uint16_t check = 0;
  std::uint16_t pos = 0;

  unsigned int request_length = build_request(
      request, modbus::function::write_multiple_coils, address, quantity);

  // set byte count
  std::uint8_t byte_count =
      static_cast<std::uint8_t>((quantity / 8) + ((quantity % 8) ? 1 : 0));
  request[request_length++] = byte_count;

  for (std::uint8_t i = 0; i < byte_count; ++i) {
    std::uint16_t bit;

    bit = 0x01;
    request[request_length] = 0;

    while ((bit & 0xFF) && (check++ < quantity)) {
      if (value[pos++]) {
        request[request_length] |= bit;
      } else {
        request[request_length] &= ~bit;
      }

      bit = static_cast<std::uint16_t>(bit << 1);
    }

    request_length++;
  }

  auto&& response = send(request, request_length);

  if (error(response)) {
    error_callback()(get_error(response));
  } else {
    response_callback()(get_response(response));
  }

  return std::move(response);
}

Modbus::Response Modbus::write_registers(const std::uint16_t& address,
                                         const std::uint16_t& quantity,
                                         const std::uint16_t* value) {
  if (!check_quantity(quantity, MAX_READ_REGISTERS)) {
    LOG_ERROR("Too many registers that need to write");
    return ModbusError{
        modbus::exception::bad_data_size,
        Modbus::exception_message(modbus::exception::bad_data_size),
        /** internal */ true};
  }

  LOG_INFO("Write registers with address={}", address);

  Buffer request;

  unsigned int request_length = build_request(
      request, modbus::function::write_multiple_registers, address, quantity);

  // set byte count
  std::uint8_t byte_count = static_cast<std::uint8_t>(quantity * 2);
  request[request_length++] = byte_count;

  for (std::uint16_t i = 0; i < quantity; ++i) {
    request[request_length++] = value[i] >> 8;
    request[request_length++] = value[i] & 0x00FF;
  }

  auto&& response = send(request, request_length);

  if (error(response)) {
    error_callback()(get_error(response));
  } else {
    response_callback()(get_response(response));
  }

  return std::move(response);
}

Modbus::Response Modbus::mask_write_register(const std::uint16_t& address,
                                             const std::uint16_t& and_mask,
                                             const std::uint16_t& or_mask) {
  LOG_INFO("Write mask register with address={}", address);

  Buffer       request;
  unsigned int request_length =
      build_request(request, modbus::function::mask_write_register, address, 0);

  request_length -= 2;

  request[request_length++] = and_mask >> 8;
  request[request_length++] = and_mask & 0x00ff;
  request[request_length++] = or_mask >> 8;
  request[request_length++] = or_mask & 0x00ff;

  auto&& response = send(request, request_length);

  if (error(response)) {
    error_callback()(get_error(response));
  } else {
    response_callback()(get_response(response));
  }

  return std::move(response);
}

Modbus::Response Modbus::read_write_registers(
    const std::uint16_t& write_address,
    const std::uint16_t& write_quantity,
    const std::uint16_t* value,
    const std::uint16_t& read_address,
    const std::uint16_t& read_quantity,
    Buffer16&            buffer) {
  if (!check_quantity(write_quantity, MAX_RW_WRITE_REGISTERS)) {
    LOG_ERROR("Too many registers that need to write");
    return ModbusError{
        modbus::exception::bad_data_size,
        Modbus::exception_message(modbus::exception::bad_data_size),
        /** internal */ true};
  }

  if (!check_quantity(read_quantity, MAX_RW_READ_REGISTERS)) {
    LOG_ERROR("Too many input registers requested");
    return ModbusError{
        modbus::exception::bad_data_size,
        Modbus::exception_message(modbus::exception::bad_data_size),
        /** internal */ true};
  }

  LOG_INFO(
      "Read and write register with write_address={}, write_quantity={}, "
      "read_address={}, read_quantity={}",
      write_address, write_quantity, read_address, read_quantity);

  Buffer       request;
  std::uint8_t byte_count = static_cast<std::uint8_t>(write_quantity * 2);
  unsigned int request_length =
      build_request(request, modbus::function::read_write_multiple_registers,
                    read_address, read_quantity);

  request[request_length++] = write_address >> 8;
  request[request_length++] = write_address & 0x00ff;
  request[request_length++] = write_quantity >> 8;
  request[request_length++] = write_quantity & 0x00ff;
  request[request_length++] = byte_count;

  for (std::uint16_t i = 0; i < write_quantity; ++i) {
    request[request_length++] = value[i] >> 8;
    request[request_length++] = value[i] & 0x00FF;
  }

  auto&& response = send(request, request_length);

  if (error(response)) {
    error_callback()(get_error(response));
  } else {
    auto& res = get_response(response);
    res.data = Buffer16{};
    auto&& data = std::get<Buffer16>(res.data);

    unsigned int offset = header_length();

    for (size_t i = 0; i < res.num_of_bytes; ++i) {
      data[i] = uint8_to_uint16(res.response,
                                offset + 2 + static_cast<unsigned int>(i << 1));
    }

    buffer = data;
    response_callback()(res);
  }

  return std::move(response);
}

void Modbus::error_callback(const Modbus::ErrorCallback& error_cb) {
  LOG_INFO("Setting Modbus error callback");
  error_callback_ = error_cb;
}

void Modbus::response_callback(const Modbus::ResponseCallback& response_cb) {
  LOG_INFO("Setting Modbus response callback");
  response_callback_ = response_cb;
}

void Modbus::callback(const Modbus::ResponseCallback& response_cb,
                      const Modbus::ErrorCallback&    error_cb) {
  LOG_INFO("Setting Modbus response and error callback");
  response_callback_ = response_cb;
  error_callback_ = error_cb;
}

void Modbus::callback(const Modbus::ErrorCallback&    error_cb,
                      const Modbus::ResponseCallback& response_cb) {
  callback(response_cb, error_cb);
}

void Modbus::connect_timeout(const Modbus::Timeout& timeout) {
  LOG_INFO("Setting Modbus connect to server timeout");
  connect_timeout_ = timeout;
}

void Modbus::request_timeout(const Modbus::Timeout& timeout) {
  LOG_INFO("Setting Modbus request timeout");
  request_timeout_ = timeout;
}

void Modbus::response_timeout(const Modbus::Timeout& timeout) {
  LOG_INFO("Setting Modbus response timeout");
  response_timeout_ = timeout;
}

bool Modbus::error(const Modbus::Response& response) {
  return std::holds_alternative<ModbusError>(response);
}

bool Modbus::succeed(const Modbus::Response& response) {
  return std::holds_alternative<ModbusResponse>(response);
}

const ModbusResponse& Modbus::get_response(const Modbus::Response& response) {
  massert(succeed(response), "sanity");
  auto pval = std::get_if<ModbusResponse>(&response);
  return *pval;
}

ModbusResponse& Modbus::get_response(Modbus::Response& response) {
  massert(succeed(response), "sanity");
  auto pval = std::get_if<ModbusResponse>(&response);
  return *pval;
}

const ModbusError& Modbus::get_error(const Modbus::Response& response) {
  massert(error(response), "sanity");
  auto pval = std::get_if<ModbusError>(&response);
  return *pval;
}

ModbusError& Modbus::get_error(Modbus::Response& response) {
  massert(error(response), "sanity");
  auto pval = std::get_if<ModbusError>(&response);
  return *pval;
}
}  // namespace networking

NAMESPACE_END
