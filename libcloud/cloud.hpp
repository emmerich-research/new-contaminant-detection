#ifndef LIB_CLOUD_CLOUD_HPP_
#define LIB_CLOUD_CLOUD_HPP_

/** @file cloud.hpp
 *  @brief Precompiled header for faster project compilation
 *
 * Every source file must include this file whether precompiled feature is
 * enabled or not
 */
#pragma GCC system_header

// 1. STL
#include <cstdint>
#include <cstdlib>
#include <string>
#include <thread>
#include <utility>

// 2. Vendor
#include <boost/core/noncopyable.hpp>

#include <tao/pq.hpp>

#include <google/cloud/internal/getenv.h>
#include <google/cloud/internal/random.h>
#include <google/cloud/storage/client.h>

// 3. Internal Project
#include <libutil/util.hpp>

#include <libcore/core.hpp>

#include <libstorage/storage.hpp>

// 4. Local
#include "config.hpp"

#include "database.hpp"

#include "storage.hpp"

#include "listener.hpp"

#endif  // LIB_CLOUD_CLOUD_HPP_
