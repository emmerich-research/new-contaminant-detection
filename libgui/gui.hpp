#ifndef APP_PRECOMPILED_HPP_
#define APP_PRECOMPILED_HPP_

/** @file precompiled.hpp
 *  @brief Precompiled header for faster project compilation
 *
 * Every source file must include this file whether precompiled feature is
 * enabled or not
 */

#pragma GCC system_header

// 1. STD
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

// 2. Vendors
// 2.1. ImGui
#include <imgui.h>

#include <imconfig.h>
// #include <imgui_internal.h>
#include <imstb_rectpack.h>
#include <imstb_truetype.h>

#include <examples/imgui_impl_glfw.h>

#include <examples/imgui_impl_opengl3.h>

// 2.2. Glad
#include <glad/glad.h>

// 2.3. GLFW
#include <GLFW/glfw3.h>

// 2.4. OpenCV
#include <opencv4/opencv2/opencv.hpp>

// 3. Internal projects
#include <libcore/core.hpp>
#include <libutil/util.hpp>

// 4. Local
#include "manager.hpp"

#include "image_texture.hpp"
#include "image_window.hpp"

#endif  // APP_PRECOMPILED_HPP_
