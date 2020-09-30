#include "server.hpp"

#include "slave.hpp"

#include <chrono>
#include <ctime>
#include <functional>
#include <string>

#include "logger.hpp"

NAMESPACE_BEGIN

namespace server {
Slave::Slave(const Config* config) : config_{config} {
  init();
}

Slave::~Slave() {
  stop();
}

void Slave::init() {
  using namespace std::placeholders;

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
          modbus::address_t{config_->plc_data().meta.starting_address},
          config_->plc_data().meta.capacity, 0},
      modbus::block::registers::initializer_t{
          modbus::address_t{config_->jetson_data().meta.starting_address},
          config_->jetson_data().meta.capacity, 0}});

  server_ = modbus::server::create(std::move(data_table));
  server_->bind_connect(std::bind(&Slave::on_connect, this, _1, _2));
  server_->bind_disconnect(std::bind(&Slave::on_disconnect, this, _1, _2));
}

void Slave::on_connect(modbus::server::session_ptr_t& session,
                       modbus::table&                 table) {
  using namespace std::chrono_literals;
  // heartbeat
  session->start_timer(1, 780ms, [this, &table]() {
    modbus::address_t address{config_->jetson_status(HEARTBEAT_KEY).address};
    table.discrete_inputs().set(address, !table.discrete_inputs().get(address));
  });

  // time
  session->start_timer(2, 1s, [this, &table]() {
    std::time_t t = std::time(nullptr);
    std::tm*    now = std::localtime(&t);

    modbus::address_t year_address{config_->jetson_data("year").address};
    modbus::address_t month_address{config_->jetson_data("month").address};
    modbus::address_t day_address{config_->jetson_data("day").address};
    modbus::address_t hour_address{config_->jetson_data("hour").address};
    modbus::address_t minute_address{config_->jetson_data("minute").address};
    modbus::address_t second_address{config_->jetson_data("second").address};
    table.input_registers().set(
        year_address, static_cast<std::uint16_t>(now->tm_year + 1900));
    table.input_registers().set(month_address,
                                static_cast<std::uint16_t>(now->tm_mon + 1));
    table.input_registers().set(day_address,
                                static_cast<std::uint16_t>(now->tm_mday));
    table.input_registers().set(hour_address,
                                static_cast<std::uint16_t>(now->tm_hour));
    table.input_registers().set(minute_address,
                                static_cast<std::uint16_t>(now->tm_min));
    table.input_registers().set(second_address,
                                static_cast<std::uint16_t>(now->tm_sec));
  });
}

void Slave::on_disconnect(
    [[maybe_unused]] modbus::server::session_ptr_t& session,
    [[maybe_unused]] modbus::table&                 table) {}

void Slave::run() {
  server_->run("0.0.0.0", std::to_string(config_->port()).c_str());
}

void Slave::stop() {
  server_->stop();
}
}  // namespace server

NAMESPACE_END
