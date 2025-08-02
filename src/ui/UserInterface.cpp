#include <ui/UserInterface.hpp>
#include <imgui.h>

namespace echonous {

UserInterface::UserInterface() {
    auto &style = ImGui::GetStyle();
    style.WindowBorderSize = 0.0f;
}

void UserInterface::draw(AppState& state) {
    drawMenuBar();
    drawSidebar(state);
    drawCurrentFileInfo(state);
}

void UserInterface::drawMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open Clip")) {
                // TODO: Open a file dialog
            }
            if (ImGui::MenuItem("Export")) {
                // TODO: Export the current state
            }
            ImGui::EndMenu();
        }
    }
    ImGui::EndMainMenuBar();
}

void UserInterface::drawSidebar(AppState& state) {
    const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
    const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();

    // Draw attached to left side of screen, full height
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 workPosition = viewport->WorkPos;
    ImVec2 workSize = viewport->WorkSize;
    float workHeight = workSize.y;

    ImGui::SetNextWindowPos(ImVec2(workPosition.x, workPosition.y), ImGuiCond_Always, ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSizeConstraints(ImVec2(0, workHeight), ImVec2(workSize.x, workHeight));

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;
    ImGui::Begin("Sidebar", nullptr, windowFlags);

    static ImGuiTableFlags table_flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;

    static ImGuiTreeNodeFlags tree_node_flags_base = ImGuiTreeNodeFlags_SpanAllColumns | ImGuiTreeNodeFlags_DefaultOpen;
    ImGui::CheckboxFlags("ImGuiTreeNodeFlags_SpanFullWidth",  &tree_node_flags_base, ImGuiTreeNodeFlags_SpanFullWidth);
    ImGui::CheckboxFlags("ImGuiTreeNodeFlags_SpanLabelWidth",  &tree_node_flags_base, ImGuiTreeNodeFlags_SpanLabelWidth);
    ImGui::CheckboxFlags("ImGuiTreeNodeFlags_SpanAllColumns", &tree_node_flags_base, ImGuiTreeNodeFlags_SpanAllColumns);
    ImGui::CheckboxFlags("ImGuiTreeNodeFlags_LabelSpanAllColumns", &tree_node_flags_base, ImGuiTreeNodeFlags_LabelSpanAllColumns);

    ImGui::BeginTable("sidebar_file_list", 3, table_flags);
    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
    ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 12.0f);
    ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 20.0f);
    ImGui::TableHeadersRow();

    ImGui::TableNextRow();
    ImGui::EndTable();

    ImGui::End();
}

void UserInterface::drawCurrentFileInfo(AppState& state) {
    ImGui::Begin("Current File Info");
    if (state.renderState.currentFilePath) {
        ImGui::Text("Current File: %s", state.renderState.currentFilePath->c_str());
    }
    else {
        ImGui::Text("No file selected");
    }

    if (state.renderState.currentImageSequence) {
        ImageSequence* imageSequence = state.renderState.currentImageSequence.value();
        ImGui::Text("Frame count: %d", imageSequence->frameCount());
        int numBytes = imageSequence->numBytes();
        // convert into user friendly KiB or MiB
        if (numBytes < 1024) {
            ImGui::Text("Size: %d bytes", numBytes);
        }
        else if (numBytes < 1024 * 1024) {
            ImGui::Text("Size: %0.2f KiB", numBytes / 1024.0f);
        }
        else {
            ImGui::Text("Size: %0.2f MiB", numBytes / 1024.0f / 1024.0f);
        }
        ImGui::Text("FPS: %0.2f", imageSequence->fps());
        ImGui::Text("Type: %.*s", static_cast<int>(format_as(imageSequence->dataType()).size()), format_as(imageSequence->dataType()).data());
        ImGui::Text("Image dimensions: %s", format_as(imageSequence->imageSize()).c_str());
        ImGui::Text("Value range: [%.2f, %.2f]", imageSequence->minValue(), imageSequence->maxValue());
    }

    ImGui::End();
}

} // namespace echonous