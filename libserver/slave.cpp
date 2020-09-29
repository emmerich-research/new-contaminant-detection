#include "server.hpp"

#include "slave.hpp"

#include <chrono>
#include <string>

#include "logger.hpp"

NAMESPACE_BEGIN

namespace server {
Slave::Slave(const Config* config) : config_{config} {
  init();
}

void Slave::init() {
  using namespace std::chrono_literals;

  modbus::logger::create<server::internal::Logger>(
      config_->base_config()->debug());

  auto&& data_table = modbus::table::create(modbus::table::initializer_t{
      modbus::block::bits::initializer_t{
          modbus::address_t{config_->plc_status().meta.starting_address},
          config_->plc_status().meta.capacity, false},
      modbus::block::bits::initializer_t{
          modbus::address_t{config_->jetson_status().meta.starting_address},
          config_->jetson_status().meta.capacity, false},
      modbus::block::registers::initializer_t{
          modbus::address_t{config_->jetson_data().meta.starting_address},
          config_->jetson_data().meta.capacity, 0},
      modbus::block::registers::initializer_t{
          modbus::address_t{config_->plc_data().meta.starting_address},
          config_->plc_data().meta.capacity, 0}});

  server_ = modbus::server::create(std::move(data_table));

  server_->bind_connect([this](auto& session_ptr, auto& table) {
    // heartbeat --
    session_ptr->start_timer(1, 780ms, [this, &table]() {
      modbus::address_t address{config_->jetson_status("heartbeat").address};
      table.discrete_inputs().set(address,
                                  !table.discrete_inputs().get(address));
    });
  });
}

void Slave::run() {
  server_->run("0.0.0.0", std::to_string(config_->port()).c_str());
}
}  // namespace server

NAMESPACE_END
