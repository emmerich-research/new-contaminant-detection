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
#include <chrono>
#include <cstdint>
#include <functional>
#include <limits>
#include <memory>
#include <optional>
#include <variant>

// 2. Vendor
// 2.1. Boost
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>

#include <boost/lambda/lambda.hpp>

// #include <boost/date_time/posix_time/posix_time_types.hpp>

#include <boost/asio.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>

// 3. Internal Project
#include <libutil/util.hpp>

#include <libcore/core.hpp>

// 4. Local
#include "modbus.hpp"

#include "modbus-tcp.hpp"

#endif  // LIB_NETWORKING_NETWORKING_HPP_
