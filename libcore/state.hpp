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
  using StatusTable = std::unordered_map<std::string, bool>;
  using DataTable = std::unordered_map<std::string, unsigned int>;

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
   * Get status table
   */
  StatusTable& status_table();
  /**
   * Get entry from status table
   *
   * @param id id of entry
   *
   * @return status of specified entry id in status table
   */
  bool status_table(const std::string& id);
  /**
   * Create/Update status table entry
   *
   * @param id    id of entry
   * @param value new status value for specific id
   */
  void status_table(const std::string& id, bool value);
  /**
   * Get data table
   */
  DataTable& data_table();
  /**
   * Get entry from data table
   *
   * @param id id of entry
   *
   * @return value of specified entry id in data table
   */
  unsigned int data_table(const std::string& id);
  /**
   * Create/Update data table entry
   *
   * @param id    id of entry
   * @param value new data value for specific id
   */
  void data_table(const std::string& id, unsigned int value);

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
   * Status table for and from PLC
   */
  StatusTable status_table_;
  /**
   * Data table from PLC
   */
  DataTable data_table_;
};
}  // namespace impl

/** impl::StateImpl singleton class using StaticObj */

NAMESPACE_END

#endif
