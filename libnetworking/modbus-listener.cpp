#include "networking.hpp"

#include "modbus-listener.hpp"

#include <functional>
#include <type_traits>
#include <variant>

#include <libutil/util.hpp>

#include "modbus-tcp.hpp"

NAMESPACE_BEGIN

namespace networking {
ModbusListener::ModbusListener(const ModbusConfig& config, bool autorun)
    : config_{std::move(config)}, running_{false} {
  const auto& timeout = config.timeout();

  modbus_ = new modbus::TCP(config.host().c_str(), config.port(),
                            timeout.connect, timeout.request, timeout.response);
  modbus()->error_callback(
      std::bind(&ModbusListener::error_callback, this, std::placeholders::_1));
  modbus()->response_callback(std::bind(&ModbusListener::response_callback,
                                        this, std::placeholders::_1));

  if (autorun) {
    start();
  }
}

ModbusListener::~ModbusListener() {
  delete modbus();
}

void ModbusListener::start() {
  std::lock_guard<std::mutex> lock(mutex());
  if (!running()) {
    LOG_INFO("Starting modbus listener");
    running_ = true;
    thread_ = std::thread(&ModbusListener::execute, this);
    // cv().notify_one();
  }
}

void ModbusListener::stop() {
  std::lock_guard<std::mutex> lock(mutex());
  if (running()) {
    LOG_INFO("Stopping modbus listener");
    running_ = false;
    thread().join();
    // cv().notify_one();
  }
}

void ModbusListener::response_callback(const ModbusResponse& response) {
  massert(State::get() != nullptr, "sanity");

  auto*        state = State::get();
  unsigned int skip = 0;

  if (response.function == modbus::function::read_input_registers) {
    // data
    if (auto value = std::get_if<Modbus::Buffer16>(&response.data)) {
      const auto& buffer = *value;
      for (const auto& [key, metadata] : config().data()) {
        const auto& type = metadata.type;

        if (type == modbus::DataType::BYTE || type == modbus::DataType::WORD) {
          state->data_table(key, buffer[skip]);
        } else if (type == modbus::DataType::DWORD) {
          state->data_table(
              key,
              util::convert_bits<std::uint16_t, std::uint32_t>(buffer, skip));
        } else if (type == modbus::DataType::LWORD) {
          state->data_table(key, buffer[skip]);
        }

        skip += util::to_underlying(type);
      }
    }
  } else if (response.function == modbus::function::read_discrete_inputs) {
    // status
    if (auto value = std::get_if<Modbus::Buffer8>(&response.data)) {
      const auto& buffer = *value;
      for (const auto& [key, metadata] : config().plc_jetson_comm()) {
        state->status_table(key, buffer[skip] == 0 ? false : true);
        skip += 1;
      }
    }
  }
}

void ModbusListener::error_callback(const ModbusError& error) {
  LOG_ERROR("Failed to get response, exception: {}, internal: {}, message: {}",
            error.exception, error.internal, error.error);
}

void ModbusListener::execute() {
  massert(State::get() != nullptr, "sanity");

  Modbus::ErrorCode ec;
  int               trial = 0;

  LOG_INFO("Connecting to modbus server...");
  ec = modbus()->connect();
  while (running() && ec && trial < 10) {
    LOG_ERROR("Error while connecting to modbus server, message: {}",
              ec.message());
    LOG_ERROR("Retrying in 5s");
    sleep_for<time_units::seconds>(5);
    ec = modbus()->connect();
    trial++;
  }

  if (!running() || ec || trial >= 10) {
    return;
  }

  while (running()) {
    // load data
    modbus()->read_input_registers(config().min_data_address(),
                                   config().data_length());

    // load plc to jetson status
    modbus()->read_input_bits(config().min_plc_jetson_comm_address(),
                              config().plc_jetson_comm_length());

    // sleep for 1s
    sleep_for<time_units::seconds>(1);
  }

  trial = 0;
  ec = modbus()->close();
  while (running() && ec && trial < 10) {
    LOG_ERROR("Error while closing connection to modbus server, message: {}",
              ec.message());
    LOG_ERROR("Retrying in 5s");
    sleep_for<time_units::seconds>(5);
    ec = modbus()->close();
    trial++;
  }
}
}  // namespace networking

NAMESPACE_END
