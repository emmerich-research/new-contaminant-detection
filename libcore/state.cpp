#include "core.hpp"

#include "state.hpp"

NAMESPACE_BEGIN

namespace impl {
StateImpl::StateImpl() {
  DEBUG_ONLY(obj_name_ = "StateImpl");
}

StateImpl::~StateImpl() {}

StateImpl::StateMutex& StateImpl::mutex() {
  return mutex_;
}

StateImpl::StatusTable& StateImpl::status_table() {
  StateImpl::StateLock lock(mutex());
  return status_table_;
}

bool StateImpl::status_table(const std::string& id) {
  StateImpl::StateLock lock(mutex());
  return status_table_[id];
}

void StateImpl::status_table(const std::string& id, bool value) {
  status_table_[id] = value;
}

StateImpl::DataTable& StateImpl::data_table() {
  StateImpl::StateLock lock(mutex());
  return data_table_;
}

int StateImpl::data_table(const std::string& id) {
  StateImpl::StateLock lock(mutex());
  return data_table_[id];
}

void StateImpl::data_table(const std::string& id, int value) {
  data_table_[id] = value;
}
}  // namespace impl

NAMESPACE_END
