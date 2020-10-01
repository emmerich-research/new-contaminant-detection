#ifndef LIB_CORE_STATE_HPP_
#define LIB_CORE_STATE_HPP_

/** @file state.hpp
 *  @brief State singleton class definition
 *
 * Hold all machine's state
 */

#include <shared_mutex>
#include <thread>
#include <unordered_map>
#include <utility>

#include <libutil/util.hpp>

#include "common.hpp"

#include "allocation.hpp"

NAMESPACE_BEGIN

// forward declaration
struct Coordinate;
struct TaskState;
namespace impl {
class StateImpl;
}

using State = StaticObj<impl::StateImpl>;

namespace impl {
/**
 * @brief State implementation.
 *        This is a class wrapper that should not be instantiated and accessed
 * publicly.
 *
 * Global state of machine (not machine state)
 *
 * @author Ray Andrew
 * @date   April 2020
 */
class StateImpl : public StackObj {
  template <class StateImpl>
  template <typename... Args>
  friend ATM_STATUS StaticObj<StateImpl>::create(Args&&... args);

 public:
  using StateMutex = std::shared_mutex;
  using StateLock = std::lock_guard<StateMutex>;

 public:
  /**
   * Get mutex
   *
   * @return state mutex
   */
  StateMutex& mutex();

 private:
  /**
   * StateImpl Constructor
   */
  explicit StateImpl();
  /**
   * StateImpl Destructor
   */
  ~StateImpl();

 private:
  /**
   *  Mutex for locking
   */
  StateMutex mutex_;
};
}  // namespace impl

/** impl::StateImpl singleton class using StaticObj */

NAMESPACE_END

#endif
