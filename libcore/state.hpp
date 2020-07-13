#ifndef LIB_CORE_STATE_HPP_
#define LIB_CORE_STATE_HPP_

/** @file state.hpp
 *  @brief State singleton class definition
 *
 * Hold all machine's state
 */

#include <shared_mutex>
#include <thread>
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

 private:
  /**
   * StateImpl Constructor
   *
   * @todo implemented soon
   */
  explicit StateImpl();
  /**
   * StateImpl Destructor
   *
   * Noop
   */
  ~StateImpl();
  /**
   * Imaging request
   *
   * @return imaging request status
   */
  bool imaging_request();
  /**
   * Imaging request
   *
   * @param imaging request status
   */
  void imaging_request(bool request_status);
  /**
   * Imaging  status
   *
   * @return imaging status
   */
  bool imaging();
  /**
   * Imaging  status
   *
   * @param new imaging status
   */
  void imaging(bool status);

 private:
  /**
   * Get mutex
   *
   * @return state mutex
   */
  StateMutex& mutex();

 private:
  /**
   *  Mutex for locking
   */
  StateMutex mutex_;
  /**
   * Imaging request
   */
  bool imaging_request_;
  /**
   * Imaging status
   */
  bool imaging_;
};
}  // namespace impl

/** impl::StateImpl singleton class using StaticObj */

NAMESPACE_END

#endif
