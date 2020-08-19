#include "modbus.hpp"

#include "modbus-connection-manager.hpp"

namespace modbus {
namespace internal {
connection_manager::connection_manager()
{
}

connection_manager::~connection_manager() {
  if (connections_.empty()) {
    stop_all();
  }
}

void connection_manager::start(connection::pointer conn) {
  connections_.insert(conn);
  conn->start();
}

void connection_manager::stop(connection::pointer conn) {
  connections_.erase(conn);
  conn->stop();
}

void connection_manager::stop_all()
{
  for (auto conn : connections_)
    conn->stop();
  connections_.clear();
}
}  // namespace internal
}  // namespace modbus
