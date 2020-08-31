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
#include <initializer_list>
#include <memory>
#include <mutex>
#include <set>
#include <shared_mutex>
#include <string>
#include <string_view>
#include <thread>
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
#include <boost/core/noncopyable.hpp>

#include <boost/bind.hpp>

#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>

#include <boost/lambda/lambda.hpp>

#include <boost/endian/conversion.hpp>

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
#include <asio2/tcp/tcp_server.hpp>

// 2.4 Struc
#include <struc.hpp>

// 3. Local
#include "modbus-constants.hpp"
#include "modbus-types.hpp"
#include "modbus-utilities.hpp"

#include "modbus-logger.hpp"

#include "modbus-exception.hpp"

#include "modbus-data-table.hpp"
#include "modbus-data-table.inline.hpp"

#include "modbus-operation.hpp"

#include "modbus-adu.hpp"
#include "modbus-request.hpp"
#include "modbus-response.hpp"

// Specific implementation
#include "modbus-bit-read.hpp"
#include "modbus-bit-read.inline.hpp"

#include "modbus-bit-write.hpp"

#include "modbus-register-read.hpp"
#include "modbus-register-read.inline.hpp"

#include "modbus-register-write.hpp"

#include "modbus-request-handler.hpp"

#include "modbus-server.hpp"

#endif  // LIB_MODBUS_MODBUS_HPP_
