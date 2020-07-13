#ifndef LIB_DETECTOR_DETECTOR_HPP_
#define LIB_DETECTOR_DETECTOR_HPP_

/** @file core.hpp
 *  @brief Precompiled header for faster project compilation
 *
 * Every source file must include this file whether precompiled feature is
 * enabled or not
 */

#pragma GCC system_header

// 1. STL
#include <algorithm>
#include <functional>
#include <iterator>
#include <numeric>
#include <optional>
#include <tuple>
#include <utility>

// 2. Vendors
// 2.1. OpenCV
#include <opencv4/opencv2/opencv.hpp>

// 3. Inside project
#include <libcore/core.hpp>
#include <libutil/util.hpp>

// 4. Local
#include "base.hpp"

#include "blob.hpp"

#include "util.hpp"
#include "util.inline.hpp"

#endif  // LIB_DETECTOR_DETECTOR_HPP_
