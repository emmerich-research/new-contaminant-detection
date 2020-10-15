#include "cloud.hpp"

#include "config.hpp"

#include <cstdlib>

namespace toml {}

NAMESPACE_BEGIN

namespace cloud {
Config::Config(const impl::ConfigImpl* config) : base_config_{config} {
  massert(config != nullptr, "sanity");
  load();
}

Config::~Config() {
  // unset env
  unsetenv("PGPASSWORD");
  unsetenv("GOOGLE_CLOUD_PROJECT");
  unsetenv("GOOGLE_APPLICATION_CREDENTIALS");
}

void Config::load() {
  name_ = base_config()->find<std::string>("cloud", "name");
  load_database();
  load_storage();

  // set env
  setenv("PGPASSWORD", database_pass().c_str(), true);
  setenv("GOOGLE_CLOUD_PROJECT", name().c_str(), true);
  setenv("GOOGLE_APPLICATION_CREDENTIALS", storage_credential().c_str(), true);
}

void Config::load_database() {
  database_.host =
      base_config()->find<std::string>("cloud", "database", "host");
  database_.port =
      base_config()->find<unsigned int>("cloud", "database", "port");
  database_.name =
      base_config()->find<std::string>("cloud", "database", "name");
  database_.user =
      base_config()->find<std::string>("cloud", "database", "user");
  database_.pass =
      base_config()->find<std::string>("cloud", "database", "password");
  database_.ssl.root_cert =
      base_config()->find<std::string>("cloud", "database", "ssl", "root-cert");
  database_.ssl.client_cert = base_config()->find<std::string>(
      "cloud", "database", "ssl", "client-cert");
  database_.ssl.client_key = base_config()->find<std::string>(
      "cloud", "database", "ssl", "client-key");
}

void Config::load_storage() {
  storage_.credential =
      base_config()->find<std::string>("cloud", "storage", "credential");
  storage_.bucket =
      base_config()->find<std::string>("cloud", "storage", "bucket");
}
}  // namespace cloud

NAMESPACE_END
