#ifndef LIB_GUI_IMAGE_WINDOW_HPP_
#define LIB_GUI_IMAGE_WINDOW_HPP_

#include <libcore/core.hpp>

#include "window.hpp"

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
   * Process image frame
   */
  void frame(const cv::Mat& image);
  /**
   * Get frames (const)
   */
  inline const cv::Mat& frame() const { return frame_; }
  /**
   * Get frames
   */
  inline cv::Mat& frame() { return frame_; }

 private:
  /**
   * Frame container
   */
  cv::Mat frame_;
};
}  // namespace gui

NAMESPACE_END

#endif  // LIB_GUI_IMAGE_WINDOW_HPP_
