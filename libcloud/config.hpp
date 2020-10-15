#ifndef LIB_CLOUD_CONFIG_HPP_
#define LIB_CLOUD_CONFIG_HPP_

/** @file config.hpp
 *  @brief Cloud config implementation
 *
 * Cloud config Implementation
 */

#include <cstdint>
#include <string>
#include <utility>

#include <libcore/core.hpp>

NAMESPACE_BEGIN

namespace cloud {
namespace config {
struct database {
  struct ssl_t {
    std::string root_cert;
    std::string client_cert;
    std::string client_key;
  };

  std::string      host;
  unsigned int     port;
  std::string      name;
  std::string      user;
  std::string      pass;
  ssl_t            ssl;
};

struct storage {
  std::string credential;
  std::string bucket;
};
}  // namespace config

class Config {
 public:
  /**
   * Cloud specialized configuration
   *
   * @param config    base config
   */
  Config(const impl::ConfigImpl* config);

  /**
   * Cloud specialized configuration copy constructor
   */
  Config(const Config&) = default;

  /**
   * Cloud config destructor
   */
  ~Config();

  /**
   * Get base config pointer
   *
   * @return base config pointer
   */
  inline const impl::ConfigImpl* base_config() const { return base_config_; }

  /**
   * Get name of cloud project
   *
   * @return name of cloud project
   */
  inline const std::string& name() const { return name_; }

  /**
   * Get database config
   *
   * @return database config
   */
  inline const config::database& database() const { return database_; }

  /**
   * Get database host
   *
   * @return database host
   */
  inline const std::string& database_host() const { return database().host; }

  /**
   * Get database port
   *
   * @return database port
   */
  inline unsigned int database_port() const { return database().port; }

  /**
   * Get database name
   *
   * @return database name
   */
  inline const std::string& database_name() const { return database().name; }

  /**
   * Get database user
   *
nnn   * @return database user
   */
  inline const std::string& database_user() const { return database().user; }

  /**
   * Get database pass
   *
   * @return database pass
   */
  inline const std::string& database_pass() const { return database().pass; }

  /**
   * Get database ssl
   *
   * @return database ssl
   */
  inline const config::database::ssl_t& database_ssl() const {
    return database().ssl;
  }

  /**
   * Get storage config
   *
   * @return storage config
   */
  inline const config::storage& storage() const { return storage_; }

  /**
   * Get storage credential
   *
   * @return storage credential
   */
  inline const std::string& storage_credential() const {
    return storage().credential;
  }

  /**
   * Get storage bucket
   *
   * @return storage bucket name
   */
  inline const std::string& storage_bucket() const { return storage().bucket; }

 private:
  /**
   * Load config
   */
  void load();

  /**
   * Load database
   */
  void load_database();

  /**
   * Load storage
   */
  void load_storage();

 private:
  /**
   * Base config pointer
   */
  const impl::ConfigImpl* base_config_;
  /**
   * Name of project
   */
  std::string name_;
  /**
   * Database
   */
  config::database database_;
  /**
   * Storage
   */
  config::storage storage_;
};
}  // namespace cloud

NAMESPACE_END

#endif  // LIB_CLOUD_CONFIG_HPP_
