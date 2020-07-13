#include "networking.hpp"

#include "modbus-window.hpp"

NAMESPACE_BEGIN

namespace networking {
namespace modbus {
const ImVec4 ModbusWindow::label_color =
    ImVec4{1.0f, 0.0f, 1.0f, 1.0f};  // pink
const ImVec4 ModbusWindow::data_color = ImVec4{1.0f, 1.0f, 1.0f, 1.0f};  // pink
const ImVec4 ModbusWindow::inactive_color =
    static_cast<ImVec4>(ImColor::HSV(0.0f, 0.8f, 0.8f));  // red
const ImVec4 ModbusWindow::active_color =
    static_cast<ImVec4>(ImColor::HSV(2 / 7.0f, 0.8f, 0.8f));  // green

ModbusWindow::ModbusWindow(const ModbusConfig&     config,
                           float                   height,
                           float                   width,
                           const ImGuiWindowFlags& flags)
    : gui::Window{"Modbus Data", height, width, flags}, config_{config} {}

ModbusWindow::~ModbusWindow() {}

void ModbusWindow::show() {
  massert(State::get() != nullptr, "sanity");

  auto* state = State::get();

  float total_w = ImGui::GetContentRegionAvail().x - 20;

  ImGui::Text("Image Data");
  ImGui::Separator();

  for (const auto& key : config().data_keys()) {
    const auto& metadata = config().data_table(key);
    ImGui::TextColored(label_color, "%s (%d, %d)", key.c_str(),
                       metadata.address, metadata.length);
    ImGui::SameLine(total_w);
    ImGui::SetNextItemWidth(total_w);
    ImGui::TextColored(data_color, "%d", state->data_table(key));
  }

  ImGui::Separator();
  ImGui::Text("PLC to Jetson Communication");
  ImGui::Separator();

  for (const auto& key : config().plc_jetson_comm_keys()) {
    const auto& metadata = config().plc_jetson_comm_table(key);
    ImGui::TextColored(label_color, "%s (%d, %d)", key.c_str(),
                       metadata.address, metadata.length);
    ImGui::SameLine(total_w);
    ImGui::SetNextItemWidth(total_w);

    const auto& value = state->status_table(key);
    if (value) {
      ImGui::TextColored(active_color, "%d", value);
    } else {
      ImGui::TextColored(inactive_color, "%d", value);
    }
  }

  ImGui::Separator();
  ImGui::Text("Jetson to PLC Communication");
  ImGui::Separator();

  for (const auto& key : config().jetson_plc_comm_keys()) {
    const auto& metadata = config().jetson_plc_comm_table(key);
    ImGui::TextColored(label_color, "%s (%d, %d)", key.c_str(),
                       metadata.address, metadata.length);
    ImGui::SameLine(total_w);
    ImGui::SetNextItemWidth(total_w);

    const auto& value = state->status_table(key);
    if (value) {
      ImGui::TextColored(active_color, "%d", value);
    } else {
      ImGui::TextColored(inactive_color, "%d", value);
    }
  }
}
}  // namespace modbus
}  // namespace networking

NAMESPACE_END
