#include "networking.hpp"

#include "modbus-listener.hpp"

#include <functional>
#include <type_traits>
#include <variant>

#include <libutil/util.hpp>

#include "modbus-tcp.hpp"

NAMESPACE_BEGIN

namespace networking {
namespace modbus {
Listener::Listener(const ModbusConfig& config, bool autorun)
    : config_{std::move(config)}, running_{false} {
  const auto& timeout = config.timeout();

  modbus_ = new modbus::TCP(config.host().c_str(), config.port(),
                            timeout.connect, timeout.request, timeout.response);
  modbus()->error_callback(
      std::bind(&Listener::error_callback, this, std::placeholders::_1));
  modbus()->response_callback(
      std::bind(&Listener::response_callback, this, std::placeholders::_1));

  if (autorun) {
    start();
  }
}

Listener::~Listener() {
  delete modbus();
}

void Listener::start() {
  std::lock_guard<std::mutex> lock(mutex());
  if (!running()) {
    LOG_INFO("Starting modbus listener");
    running_ = true;
    thread_ = std::thread(&Listener::execute, this);
    // cv().notify_one();
  }
}

void Listener::stop() {
  std::lock_guard<std::mutex> lock(mutex());
  if (running()) {
    LOG_INFO("Stopping modbus listener");
    running_ = false;
    thread().join();
    // cv().notify_one();
  }
}

void Listener::response_callback(const ModbusResponse& response) {
  massert(State::get() != nullptr, "sanity");

  auto*       state = State::get();
  std::size_t skip = 0;

  if (response.function == modbus::function::read_input_registers) {
    // data
    if (auto value = std::get_if<Modbus::Buffer16>(&response.data)) {
      const auto& buffer = *value;
      for (const auto& [key, metadata] : config().data()) {
        state->data_table(key, buffer[skip]);
        skip += metadata.length;
      }
    }
  } else if (response.function == modbus::function::read_discrete_inputs) {
    // status
    if (auto value = std::get_if<Modbus::Buffer8>(&response.data)) {
      const auto& buffer = *value;
      for (const auto& [key, metadata] : config().plc_jetson_comm()) {
        state->status_table(key, buffer[skip] == 0 ? false : true);
        skip += metadata.length;
      }
    }
  }
}

void Listener::error_callback(const ModbusError& error) {
  LOG_ERROR("Failed to get response, exception: {}, internal: {}, message: {}",
            error.exception, error.internal, error.error);
}

void Listener::execute() {
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
    sleep_for<time_units::seconds>(5);
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
}  // namespace modbus
}  // namespace networking

NAMESPACE_END
