#include "modbus.hpp"

#include "modbus-logger.hpp"

namespace modbus {
logger* logger::instance_ = nullptr;

logger::logger(bool debug) : debug_{debug} {}

logger::~logger() {}

logger* logger::get() {
  if (instance_ == nullptr) {
    create();
  }

  return instance_;
}

void logger::info(const std::string& message) const noexcept {
  fmt::print(std::cout, "{}\n", message);
}

void logger::error(const std::string& message) const noexcept {
  info(message);
}

void logger::debug(const std::string& message) const noexcept {
  if (debug_) {
    info(message);
  }
}
}  // namespace modbus
