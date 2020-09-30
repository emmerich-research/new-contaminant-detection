#ifndef LIB_SERVER_SERVER_HPP_
#define LIB_SERVER_SERVER_HPP_

/** @file server.hpp
 *  @brief Precompiled header for faster project compilation
 *
 * Every source file must include this file whether precompiled feature is
 * enabled or not
 */
#pragma GCC system_header

// 1. STL
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <functional>
#include <limits>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

// 2. Vendor
// 2.1. ModbusCPP
#include <asio2/base/timer.hpp>
#include <modbuscpp/modbus.hpp>

// 3. Internal Project
#include <libutil/util.hpp>

#include <libcore/core.hpp>

#include <libgui/gui.hpp>

// 4. Local
#include "logger.hpp"

#include "config.hpp"

#include "slave.hpp"

#include "data-mapper.hpp"

#include "data-window.hpp"

#endif  // LIB_SERVER_SERVER_HPP_
