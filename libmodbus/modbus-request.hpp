#ifndef LIB_MODBUS_MODBUS_REQUEST_HPP_
#define LIB_MODBUS_MODBUS_REQUEST_HPP_

#include <cstdlib>
#include <type_traits>

#include "modbus-adu.hpp"
#include "modbus-constants.hpp"
#include "modbus-types.hpp"

#include "modbus-response.hpp"

namespace modbus {
// forward declarations
class table;
namespace internal {
template <constants::function_code modbus_function>
class request;
}

namespace internal {
template <constants::function_code modbus_function>
class request : public adu<modbus_function> {
 public:
  /**
   * Initializer
   */
  using typename adu<modbus_function>::initializer_t;

  /**
   * Request constructor
   */
  explicit request();

  /**
   * Request constructor
   *
   * @param transaction transaction id
   * @param unit        unit id
   */
  explicit request(std::uint16_t transaction,
                   std::uint8_t  unit);

  /**
   * Request constructor
   *
   * @param initializer initializer
   */
  explicit request(const initializer_t& initializer);

  /**
   * Execute on data store / mapping
   *
   * @param  data_table data table
   *
   * @return modbus response
   */
  virtual typename response<modbus_function>::pointer execute(
      table& data_table) = 0;

  /**
   * Get response size for error checking on client
   *
   * @return response size
   */
  inline virtual typename internal::packet_t::size_type response_size() const {
    return 0;
  }
};
}  // namespace internal
}  // namespace modbus

#endif  // LIB_MODBUS_MODBUS_REQUEST_HPP_
