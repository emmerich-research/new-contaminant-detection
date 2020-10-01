#ifndef LIB_GUI_IMAGE_WINDOW_HPP_
#define LIB_GUI_IMAGE_WINDOW_HPP_

#include <libcore/core.hpp>

#include "window.hpp"

#include "image_texture.hpp"

NAMESPACE_BEGIN

namespace gui {
class ImageWindow : public Window {
 public:
  /**
   * Image Window constructor
   *
   * @param name   window name
   * @param width  window width
   * @param height window height
   * @param flags  window flags
   */
  ImageWindow(const char*             name,
              float                   height,
              float                   width,
              const ImGuiWindowFlags& flags = 0);
  /**
   * Image Window destructor
   */
  virtual ~ImageWindow() override;
  /**
   *  Show contents
   */
  virtual void show() override;
  /**
   *  After render contents
   */
  virtual void after_render() override;
  /**
   * Process image frame
   *
   * @param image image to show
   */
  void frame(cv::Mat* image);
  /**
   * Get image texture (const)
   *
   * @return image texture (const)
   */
  inline const ImageTexture& texture() const { return texture_; }
  /**
   * Get image texture
   *
   * @return image texture
   */
  inline ImageTexture& texture() { return texture_; }

 private:
  /**
   * Image texture from OpenCV
   */
  ImageTexture texture_;
};
}  // namespace gui

NAMESPACE_END

#endif  // LIB_GUI_IMAGE_WINDOW_HPP_
