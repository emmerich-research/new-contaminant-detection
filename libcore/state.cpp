#include "core.hpp"

#include "state.hpp"

NAMESPACE_BEGIN

namespace impl {
StateImpl::StateImpl() : imaging_request_{false}, imaging_{false} {
  DEBUG_ONLY(obj_name_ = "StateImpl");
}

StateImpl::~StateImpl() {}

StateImpl::StateMutex& StateImpl::mutex() {
  return mutex_;
}

bool StateImpl::imaging_request() {
  StateImpl::StateLock lock(mutex());
  return imaging_;
}

void StateImpl::imaging_request(bool request_status) {
  StateImpl::StateLock lock(mutex());
  imaging_request_ = request_status;
}

bool StateImpl::imaging() {
  StateImpl::StateLock lock(mutex());
  return imaging_;
}

void StateImpl::imaging(bool status) {
  StateImpl::StateLock lock(mutex());
  imaging_ = status;
}
}  // namespace impl

NAMESPACE_END
