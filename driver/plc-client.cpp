#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

#include <libcore/core.hpp>
#include <libnetworking/networking.hpp>
#include <libutil/util.hpp>

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
  USE_NAMESPACE
  using namespace networking;

  if (initialize_core()) {
    std::cerr << "cannot initialize config, state, and logger!" << std::endl;
    return ATM_ERR;
  }

  networking::ModbusConfig config{Config::get()};
  const auto&              timeout = config.timeout();

  networking::Modbus::ErrorCode ec;
  networking::modbus::TCP       modbus{config.host().c_str(), config.port(),
                                 timeout.connect, timeout.request,
                                 timeout.response};

  ec = modbus.connect();

  if (ec) {
    LOG_ERROR("Connection error, message: {}", ec.message());
    return ATM_ERR;
  }

  modbus.error_callback([](const auto& error) {
    LOG_ERROR(
        "Failed to get response, exception: {}, internal: {}, message: {}",
        error.exception, error.internal, error.error);
  });

  modbus.response_callback([&config](const auto& response) {
    massert(State::get() != nullptr, "sanity");

    auto*        state = State::get();
    unsigned int skip = 0;

    if (response.function == modbus::function::read_input_registers) {
      // data
      if (auto value = std::get_if<Modbus::Buffer16>(&response.data)) {
        const auto& buffer = *value;
        for (const auto& [key, metadata] : config.data()) {
          const auto& type = metadata.type;

          if (type == modbus::DataType::BYTE ||
              type == modbus::DataType::WORD) {
            state->data_table(key, buffer[skip]);
          } else if (type == modbus::DataType::DWORD) {
            auto val = util::convert_bits<std::uint16_t, std::uint32_t>(
                buffer, skip, /* reverse */ true);
            LOG_DEBUG("Key={}, hi={}, low={}, value={}", key, buffer[skip],
                      buffer[skip + 1], val);
            state->data_table(key, val);
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
        for (const auto& [key, metadata] : config.plc_jetson_comm()) {
          state->status_table(key, buffer[skip] == 0 ? false : true);
          skip += 1;
        }
      }
    }
  });

  auto* state = State::get();
  while (true) {
    // load data
    modbus.read_input_registers(config.min_data_address(),
                                config.data_length());

    // load plc to jetson status
    modbus.read_input_bits(config.min_plc_jetson_comm_address(),
                           config.plc_jetson_comm_length());

    // sleep for 1s
    sleep_for<time_units::seconds>(2);

    std::string print = "[";

    for (const auto& [key, metadata] : config.data()) {
      auto value = state->data_table(key);

      print += fmt::format("{}={}, ", key.c_str(), value);
    }

    for (const auto& [key, metadata] : config.plc_jetson_comm()) {
      auto value = state->status_table(key);

      print += fmt::format("{}={}, ", key.c_str(), value);
    }

    for (const auto& [key, metadata] : config.jetson_plc_comm()) {
      auto value = state->status_table(key);

      print += fmt::format("{}={}, ", key.c_str(), value);
    }

    print.pop_back();
    print.pop_back();
    print += "]";
    LOG_DEBUG("{}", print);
  }

  return 0;
}
