#ifndef LIB_GUI_IMAGE_TEXTURE_HPP_
#define LIB_GUI_IMAGE_TEXTURE_HPP_

/** @file image_texture.hpp
 *  @brief Imgui OpenCV Image texture
 *
 * Imgui OpenCV Image texture
 */

#include <cstdint>
#include <string>

#include <opencv4/opencv2/opencv.hpp>

#include <external/imgui/imgui.h>

#include <libcore/core.hpp>

NAMESPACE_BEGIN

namespace gui {
/**
 * @brief OpenCV Frame Texture bridge for ImGUI
 *
 * Credits to :
 * https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples#Example-for-OpenGL-users
 * https://github.com/ashitani/opencv_imgui_viewer/blob/master/cv2viewer.cpp
 *
 * @author Ray Andrew
 * @date   July 2020
 */
class ImageTexture {
 public:
  /**
   * ImageTexture constructor
   */
  ImageTexture();
  /**
   * ImageTexture constructor
   */
  ImageTexture(const cv::Mat& frame);
  /**
   * ImageTexture destructor
   */
  ~ImageTexture();
  /**
   * Set texture from cv::Mat
   *
   * @param frame  frame to process (cv::Mat (BGR))
   */
  void image(const cv::Mat& frame);
  /**
   * Set texture from filename
   *
   * @param filename  file to process
   */
  void image(const std::string& filename);
  /**
   * Get texture
   *
   * @return pointer to texture
   */
  inline void* texture() const {
    return reinterpret_cast<void*>(static_cast<std::intptr_t>(opengl_texture_));
  }
  /**
   * Get size of image
   *
   * @return size of image
   */
  inline ImVec2 size() const { return ImVec2{width_, height_}; }

 private:
  /**
   * Image width
   */
  float width_;
  /**
   * Image height
   */
  float height_;
  /**
   * OpenGL texturee
   */
  GLuint opengl_texture_;
};
}  // namespace gui

NAMESPACE_END

#endif  // LIB_GUI_IMAGE_TEXTURE_HPP_
