#include "server.hpp"

#include "data-window.hpp"

#include <string>
#include <utility>

#include "config.hpp"
#include "data-mapper.hpp"

NAMESPACE_BEGIN

namespace server {
const ImVec4 DataWindow::label_color = ImVec4{1.0f, 0.0f, 1.0f, 1.0f};  // pink
const ImVec4 DataWindow::data_color = ImVec4{1.0f, 1.0f, 1.0f, 1.0f};   // pink
const ImVec4 DataWindow::inactive_color =
    static_cast<ImVec4>(ImColor::HSV(0.0f, 0.8f, 0.8f));  // red
const ImVec4 DataWindow::active_color =
    static_cast<ImVec4>(ImColor::HSV(2 / 7.0f, 0.8f, 0.8f));  // green

DataWindow::DataWindow(const Config*           config,
                       const DataMapper*       data_mapper,
                       float                   height,
                       float                   width,
                       const ImGuiWindowFlags& flags)
    : gui::Window{"Modbus Data", height, width, flags},
      config_{config},
      data_mapper_{data_mapper} {}

DataWindow::~DataWindow() {}

void DataWindow::show() {
  float total_w = ImGui::GetContentRegionAvail().x;

  {
    ImGui::Text("Jetson Status");
    ImGui::Separator();

    for (const auto& [key, metadata] : config()->jetson_status().info) {
      auto value =
          data_mapper()->status(mapping::alt_type_t::jetson_status, key);
      ImGui::TextColored(label_color, "%s (%d, %d)", key.c_str(),
                         metadata.address, metadata.length);
      ImGui::SameLine(total_w);
      ImGui::SetNextItemWidth(total_w);

      ImGui::TextColored(value ? active_color : inactive_color, "%d", value);
    }

    ImGui::Separator();
  }

  {
    ImGui::Text("Jetson Data");
    ImGui::Separator();

    for (const auto& [key, metadata] : config()->jetson_data().info) {
      auto value = data_mapper()->data(mapping::alt_type_t::jetson_data, key);
      ImGui::TextColored(label_color, "%s (%d, %d)", key.c_str(),
                         metadata.address, metadata.length);
      ImGui::SameLine();
      ImGui::TextColored(data_color, "%lld", value);
    }

    ImGui::Separator();
  }

  {
    ImGui::Text("PLC Status");
    ImGui::Separator();

    for (const auto& [key, metadata] : config()->plc_status().info) {
      auto value = data_mapper()->status(mapping::alt_type_t::plc_status, key);
      ImGui::TextColored(label_color, "%s (%d, %d)", key.c_str(),
                         metadata.address, metadata.length);
      ImGui::SameLine(total_w);
      ImGui::SetNextItemWidth(total_w);

      ImGui::TextColored(value ? active_color : inactive_color, "%d", value);
    }

    ImGui::Separator();
  }

  {
    ImGui::Text("PLC Data");
    ImGui::Separator();

    for (const auto& [key, metadata] : config()->plc_data().info) {
      auto value = data_mapper()->data(mapping::alt_type_t::plc_data, key);
      ImGui::TextColored(label_color, "%s (%d, %d)", key.c_str(),
                         metadata.address, metadata.length);
      ImGui::SameLine();
      ImGui::TextColored(data_color, "%lld", value);
    }

    // ImGui::Separator();
  }
}
}  // namespace server

NAMESPACE_END
