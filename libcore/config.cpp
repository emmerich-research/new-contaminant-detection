#include "core.hpp"

#include "config.hpp"

#include <utility>

NAMESPACE_BEGIN

// impl::ConfigImpl* Config::instance_ = nullptr;

namespace impl {
ConfigImpl::ConfigImpl(const std::string& config_path)
    : config_{toml::parse(config_path)}, config_path_{std::move(config_path)} {
  DEBUG_ONLY(obj_name_ = "ConfigImpl");
}

std::string ConfigImpl::name() const {
  if (config().contains("general") && config().at("general").contains("name")) {
    return find<std::string>("general", "name");
  }

  return "Emmerich";
}

bool ConfigImpl::debug() const {
  if (config().contains("general") &&
      config().at("general").contains("debug")) {
    return find<bool>("general", "debug");
  }

  return false;
}

int ConfigImpl::camera_idx() const {
  if (config().contains("camera") && config().at("camera").contains("index")) {
    return find<int>("camera", "index");
  }

  return 0;  // default to zero (default camera)
}
}  // namespace impl

NAMESPACE_END
