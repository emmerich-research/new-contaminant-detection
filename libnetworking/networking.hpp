#ifndef LIB_NETWORKING_NETWORKING_HPP_
#define LIB_NETWORKING_NETWORKING_HPP_

/** @file networking.hpp
 *  @brief Precompiled header for faster project compilation
 *
 * Every source file must include this file whether precompiled feature is
 * enabled or not
 */
#pragma GCC system_header

// 1. STL
#include <array>
#include <cstdint>
#include <limits>
#include <memory>
#include <optional>
#include <variant>

// 2. Vendor
// 2.1. Boost ASIO
#include <boost/system/error_code.hpp>

#include <boost/asio.hpp>

// 3. Internal Project
#include <libutil/util.hpp>

#include <libcore/core.hpp>

// 4. Local
#include "modbus.hpp"

#include "modbus-tcp.hpp"

#endif  // LIB_NETWORKING_NETWORKING_HPP_
