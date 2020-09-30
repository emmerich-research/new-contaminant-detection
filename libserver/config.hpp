#ifndef LIB_SERVER_CONFIG_HPP_
#define LIB_SERVER_CONFIG_HPP_

/** @file config.hpp
 *  @brief Server config implementation
 *
 * Server config Implementation
 */

#include <cstdint>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <libcore/core.hpp>

NAMESPACE_BEGIN

namespace server {
namespace data {
enum class type_t : unsigned int { byte = 0, word, dword, lword };

struct meta_t {
  type_t        type;
  std::uint16_t address;
  std::uint16_t length;
};
}  // namespace data

namespace mapping {
enum class type_t { coils, discrete_inputs, input_regs, holding_regs };
enum class alt_type_t { plc_status, jetson_status, jetson_data, plc_data };
typedef std::pair<std::string, data::meta_t> entry_t;
typedef std::vector<entry_t>                 info_t;

struct meta_t {
  std::uint16_t starting_address;
  std::uint16_t capacity;
};

struct data_t {
  type_t type;
  info_t info;
  meta_t meta;
};
}  // namespace mapping

class Config {
 public:
  /**
   * Server specialized configuration
   *
   * @param config    base config
   */
  Config(const impl::ConfigImpl* config);

  /**
   * Server specialized configuration copy constructor
   */
  Config(const Config&) = default;

  /**
   * Get base config pointer
   *
   * @return base config pointer
   */
  inline const impl::ConfigImpl* base_config() const { return base_config_; }

  /**
   * Get Jetson data mapping
   *
   * @return Jetson data mapping
   */
  inline const mapping::data_t& jetson_data() const { return jetson_data_; };

  /**
   * Get data metadata from Jetson data mapping with specified id
   *
   * @return Jetson data metadata
   */
  const data::meta_t& jetson_data(const std::string& id) const;

  /**
   * Get Jetson status mapping
   *
   * @return Jetson status mapping
   */
  inline const mapping::data_t& jetson_status() const {
    return jetson_status_;
  };

  /**
   * Get status metadata from Jetson status mapping with specified id
   *
   * @return Jetson status metadata
   */
  const data::meta_t& jetson_status(const std::string& id) const;

  /**
   * Get PLC data mapping
   *
   * @return PLC data mapping
   */
  inline const mapping::data_t& plc_data() const { return plc_data_; };

  /**
   * Get data metadata from PLC data mapping with specified id
   *
   * @return PLC data metadata
   */
  const data::meta_t& plc_data(const std::string& id) const;

  /**
   * Get PLC status mapping
   *
   * @return PLC status mapping
   */
  inline const mapping::data_t& plc_status() const { return plc_status_; };

  /**
   * Get status metadata from PLC data mapping with specified id
   *
   * @return PLC status metadata
   */
  const data::meta_t& plc_status(const std::string& id) const;

  /**
   * Get server port
   *
   * @return server port
   */
  inline std::uint16_t port() const { return port_; }

 private:
  /**
   * Load config
   */
  void load();

  /**
   * Load server info
   */
  void load_server_info();

  /**
   * Load data mapping
   */
  void load_data();

  /**
   * Load data load helper
   *
   * @param key           toml key
   * @param type          mapping type
   * @param mapping_data  data mapping to write
   */
  void load_data_helper(const char*            key,
                        const mapping::type_t& type,
                        mapping::data_t&       mapping_data);
  /**
   * Sort mapping info
   *
   * @param info    data mapping info
   */
  static void sort_mapping_info(mapping::info_t& info);

 private:
  /**
   * Base config pointer
   */
  const impl::ConfigImpl* base_config_;
  /**
   * Server port
   */
  std::uint16_t port_;
  /**
   * Jetson data
   */
  mapping::data_t jetson_data_;
  /**
   * Jetson status
   */
  mapping::data_t jetson_status_;
  /**
   * PLC data
   */
  mapping::data_t plc_data_;
  /**
   * PLC status
   */
  mapping::data_t plc_status_;
};
}  // namespace server

NAMESPACE_END

#endif  // LIB_SERVER_CONFIG_HPP_
