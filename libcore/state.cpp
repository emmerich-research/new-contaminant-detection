#include "core.hpp"

#include "state.hpp"

NAMESPACE_BEGIN

namespace impl {
StateImpl::StateImpl() {
  DEBUG_ONLY(obj_name_ = "StateImpl");
}

StateImpl::StateMutex& StateImpl::mutex() {
  return mutex_;
}
}  // namespace impl

NAMESPACE_END
