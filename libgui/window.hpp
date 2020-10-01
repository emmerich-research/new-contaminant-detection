#ifndef LIB_GUI_WINDOW_HPP_
#define LIB_GUI_WINDOW_HPP_

#include <imgui.h>

#include <libcore/core.hpp>

NAMESPACE_BEGIN

namespace gui {
class Window {
 public:
  /**
   * Window constructor
   *
   * @param name   window name
   * @param width  window width
   * @param height window height
   * @param flags  window flags
   */
  Window(const char*             name,
         float                   width,
         float                   height,
         const ImGuiWindowFlags& flags = 0);
  /**
   * Window destructor
   */
  virtual ~Window();
  /**
   * Show content of window
   */
  void render();
  /**
   * Show content of window
   */
  virtual void show() = 0;
  /**
   * Show content of window
   */
  virtual void after_render();

 protected:
  /**
   * Get name
   *
   * @return window name
   */
  inline const char* name() const { return name_; }
  /**
   * Get width
   *
   * @return window width
   */
  inline float width() const { return width_; }
  /**
   * Get height
   *
   * @return window height
   */
  inline float height() const { return height_; }
  /**
   * Get flags
   *
   * @return window flags
   */
  inline const ImGuiWindowFlags& flags() const { return flags_; }

 private:
  /**
   * Window name
   */
  const char* name_;
  /**
   * Window width
   */
  const float width_;
  /**
   * Window height
   */
  const float height_;
  /**
   * Window flags
   */
  const ImGuiWindowFlags flags_;
};
}  // namespace gui

NAMESPACE_END

#endif  // LIB_GUI_WINDOW_HPP_
