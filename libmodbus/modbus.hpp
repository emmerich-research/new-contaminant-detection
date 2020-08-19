#ifndef LIB_MODBUS_MODBUS_HPP_
#define LIB_MODBUS_MODBUS_HPP_

/** @file modbus.hpp
 *  @brief Precompiled header for faster project compilation
 *
 * Every source file must include this file whether precompiled feature is
 * enabled or not
 */

#pragma GCC system_header

// 1. STL
#include <algorithm>
#include <cstdint>
#include <exception>
#include <memory>
#include <set>
#include <string>
#include <type_traits>
#include <vector>

#include <arpa/inet.h>

// 2. Vendors
// 2.1 Fmt
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ostream.h>
#ifdef NDEBUG
#include <fmt/ranges.h>
#endif

// 2.2. Boost
#include <boost/bind.hpp>

#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>

#include <boost/lambda/lambda.hpp>

// #include <boost/date_time/posix_time/posix_time_types.hpp>

//#include <boost/asio/connect.hpp>
//#include <boost/asio/io_service.hpp>
//#include <boost/asio/ip/tcp.hpp>
//#include <boost/asio/read_until.hpp>
//#include <boost/asio/streambuf.hpp>
//#include <boost/asio/write.hpp>

// 2.3 Asio2
#include <asio2/config.hpp>
#include <asio2/version.hpp>

#include <asio2/base/timer.hpp>
#include <asio2/tcp/tcp_client.hpp>
#include <asio2/tcp/tcp_server.hpp>

// 3. Local
#include "modbus-constants.hpp"
#include "modbus-types.hpp"
#include "modbus-utilities.hpp"

#include "modbus-exception.hpp"

#include "modbus-data-table.hpp"
#include "modbus-data-table.inline.hpp"

#include "modbus-adu.hpp"
#include "modbus-adu.inline.hpp"

#include "modbus-request.hpp"
#include "modbus-request.inline.hpp"

#include "modbus-response.hpp"
#include "modbus-response.inline.hpp"

// Specific implementation
#include "modbus-bit-read.hpp"

#include "modbus-connection-manager.hpp"
#include "modbus-connection.hpp"

#include "modbus-server.hpp"

#endif  // LIB_MODBUS_MODBUS_HPP_
