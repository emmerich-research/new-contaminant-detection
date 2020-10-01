#include "server.hpp"

#include "logger.hpp"

NAMESPACE_BEGIN

namespace server {
namespace internal {
Logger::Logger(bool debug) : modbus::logger(debug) {}

Logger::~Logger() {}

void Logger::error_impl(const std::string& message) const noexcept {
  LOG_ERROR("{}", message);
}

void Logger::debug_impl(const std::string& message) const noexcept {
  if (debug_) {
    LOG_DEBUG("{}", message);
  }
}

void Logger::info_impl(const std::string& message) const noexcept {
  LOG_INFO("{}", message);
}
}  // namespace internal
}  // namespace server

NAMESPACE_END
