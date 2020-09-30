#ifndef LIB_SERVER_DATA_MAPPING_HPP_
#define LIB_SERVER_DATA_MAPPING_HPP_

/** @file data-mapping.hpp
 *  @brief Modbus data mapping
 *
 * Modbus data mapping from config to data table
 */

#include <condition_variable>
#include <type_traits>

#include <libcore/core.hpp>

#include "config.hpp"

NAMESPACE_BEGIN

namespace server {
class Slave;

class DataMapper {
 public:
  using Signal = std::condition_variable;

  /**
   * DataMapper constructor
   *
   * @param config  server config
   * @param slave   server slave
   */
  DataMapper(const Config* config, Slave* slave);

  /**
   * DataMapper destructor
   */
  ~DataMapper();

  /**
   * Get signal
   *
   * @return signal
   */
  Signal& signal();

  /**
   * Notify one thread
   */
  void notify_one();

  /**
   * Notify all threads
   */
  void notify_all();

  /**
   * Get data
   *
   * @param type  mapping type
   * @param id    data id
   * @return data
   */
  long long data(mapping::alt_type_t type, std::string id) const;

  /**
   * Set data
   *
   * @param type  mapping type
   * @param id    data id
   * @param value value of specified id of data
   */
  template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
  void data(mapping::alt_type_t type, std::string id, T value) {
    if constexpr (sizeof(T) <= 2) {
    } else if constexpr (sizeof(T) == 3) {
    } else if constexpr (sizeof(T) == 4) {
    }
  }

  /**
   * Get status
   *
   * @param type  mapping type
   * @param id    status id
   * @return status
   */
  bool status(mapping::alt_type_t type, std::string id) const;

  /**
   * Set status
   *
   * @param type  mapping type
   * @param id    status id
   * @param value value of specified id of status
   */
  void status(mapping::alt_type_t type, std::string id, bool value);

 private:
  /**
   * Config
   */
  const Config* config_;
  /**
   * Slave
   */
  Slave* slave_;
  /**
   * Signal
   */
  Signal signal_;
};
}  // namespace server

NAMESPACE_END

#endif
