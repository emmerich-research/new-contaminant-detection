#include "networking.hpp"

#include <string>
#include <utility>

#include "modbus-window.hpp"

NAMESPACE_BEGIN

namespace networking {
namespace modbus {
const ImVec4 ModbusWindow::label_color =
    ImVec4{1.0f, 0.0f, 1.0f, 1.0f};                                      // pink
const ImVec4 ModbusWindow::data_color = ImVec4{1.0f, 1.0f, 1.0f, 1.0f};  // pink
const ImVec4 ModbusWindow::inactive_color =
    static_cast<ImVec4>(ImColor::HSV(0.0f, 0.8f, 0.8f));  // red
const ImVec4 ModbusWindow::active_color =
    static_cast<ImVec4>(ImColor::HSV(2 / 7.0f, 0.8f, 0.8f));  // green

ModbusWindow::ModbusWindow(const ModbusConfig&     config,
                           float                   height,
                           float                   width,
                           const ImGuiWindowFlags& flags)
    : gui::Window{"Modbus Data", height, width, flags},
      config_{std::move(config)} {}

ModbusWindow::~ModbusWindow() {}

void ModbusWindow::show() {
  massert(State::get() != nullptr, "sanity");

  auto* state = State::get();
  // ImGuiIO& io = ImGui::GetIO();

  float total_w = ImGui::GetContentRegionAvail().x;

  {
    ImGui::Text("Image Data");
    ImGui::Separator();

    for (const auto& [key, metadata] : config().data()) {
      auto value = state->data_table(key);
      // auto value_str = std::to_string(value);
      // if (key.compare("year") == 0)
      //   LOG_INFO("{} {}", value, value_str.length());
      ImGui::TextColored(label_color, "%s (%d, %d)", key.c_str(),
                         metadata.address, metadata.length);
      ImGui::SameLine();
      // ImGui::SameLine(total_w - 20 - value_str.length() * font_size);
      // ImGui::SetNextItemWidth(total_w - 20 - value_str.length() * font_size);
      ImGui::TextColored(data_color, "%u", value);
    }

    ImGui::Separator();
  }

  {
    ImGui::Text("PLC to Jetson Communication");
    ImGui::Separator();

    for (const auto& [key, metadata] : config().plc_jetson_comm()) {
      ImGui::TextColored(label_color, "%s (%d, %d)", key.c_str(),
                         metadata.address, metadata.length);
      ImGui::SameLine(total_w);
      ImGui::SetNextItemWidth(total_w);

      const auto& value = state->status_table(key);
      ImGui::TextColored(value ? active_color : inactive_color, "%d", value);
    }

    ImGui::Separator();
  }

  {
    ImGui::Text("Jetson to PLC Communication");
    ImGui::Separator();

    for (const auto& [key, metadata] : config().jetson_plc_comm()) {
      ImGui::TextColored(label_color, "%s (%d, %d)", key.c_str(),
                         metadata.address, metadata.length);

      ImGui::SameLine(total_w);
      ImGui::SetNextItemWidth(total_w);

      const auto& value = state->status_table(key);
      ImGui::TextColored(value ? active_color : inactive_color, "%d", value);
    }
  }
}
}  // namespace modbus
}  // namespace networking

NAMESPACE_END
