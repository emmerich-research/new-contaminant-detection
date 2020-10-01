#ifndef LIB_SERVER_DATA_WINDOW_HPP_
#define LIB_SERVER_DATA_WINDOW_HPP_

#include <libcore/core.hpp>
#include <libgui/gui.hpp>

NAMESPACE_BEGIN

namespace server {
class Config;
class DataMapper;

class DataWindow : public gui::Window {
 public:
  /**
   * DataWindow constructor
   *
   * @param config server configuration
   * @param width  window width
   * @param height window height
   * @param flags  window flags
   */
  DataWindow(const Config*           config,
             const DataMapper*       data_mapper,
             float                   height = 300.0f,
             float                   width = 300.0f,
             const ImGuiWindowFlags& flags = 0);
  /**
   *  DataWindow destructor
   */
  virtual ~DataWindow() override;

  /**
   *  Show contents
   */
  virtual void show() override;

 private:
  inline const Config*     config() const { return config_; }
  inline const DataMapper* data_mapper() const { return data_mapper_; }

 private:
  static const ImVec4 label_color;
  static const ImVec4 data_color;
  static const ImVec4 inactive_color;
  static const ImVec4 active_color;
  const Config*       config_;
  const DataMapper*   data_mapper_;
};
}  // namespace server

NAMESPACE_END

#endif  // LIB_SERVER_DATA_WINDOW_HPP_
