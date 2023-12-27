#include <Editor/GUI/MainGUI.hpp>
#include <Engine/Components/IncludeComponents.hpp>
#include <icons/icons.h>

using namespace Engine::Components;

namespace Editor {
    DLL_API std::vector<Log> GUI::logs;
    DLL_API uint32_t GUI::framebufferTextureID;
    DLL_API Engine::GameObject *GUI::selected_gameObject;

    char asciitolower(char in) {
        if (in <= 'Z' && in >= 'A')
            return in - ('Z' - 'z');
        return in;
    }

    void GUI::SetNameIcon(std::string &txt, Engine::GameObject *gameObject) {
        if (gameObject->HasComponent<Camera>()) {
            txt = ICON_FA_CAMERA;
        }

        if (gameObject->HasComponent<AmbientLight>()) {
            txt = ICON_FA_STAR;
        }

        if (gameObject->HasComponent<DirectionalLight>()) {
            txt = ICON_FA_SUN;
        }

        if (gameObject->HasComponent<PointLight>()) {
            txt = ICON_FA_LIGHTBULB;
        }

        if (gameObject->HasComponent<Text3D>()) {
            txt = ICON_FA_FONT;
        }
    }

    void GUI::DrawVec3Control(const std::string &label, glm::vec3 &values, float resetValue, float columnWidth) {
        ImGui::PushID(label.c_str());
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, 88);
        ImGui::Text("%s", label.c_str());
        ImGui::NextColumn();

        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.5f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImVec2 buttonSize = {lineHeight + 2.0f, lineHeight};

        // disable button rounded corners
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                              ImVec4{0.9f, 0.2f, 0.2f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                              ImVec4{0.8f, 0.1f, 0.15f, 1.0f});

        if (ImGui::Button("X", buttonSize)) {
            values.x = resetValue;
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                              ImVec4{0.3f, 0.8f, 0.3f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                              ImVec4{0.2f, 0.7f, 0.2f, 1.0f});

        if (ImGui::Button("Y", buttonSize)) {
            values.y = resetValue;
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                              ImVec4{0.2f, 0.35f, 0.9f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                              ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
        if (ImGui::Button("Z", buttonSize)) {
            values.z = resetValue;
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();
        ImGui::Columns(1);
        ImGui::PopStyleVar(3);
        ImGui::PopID();
    }

    void GUI::DrawVec2Control(const std::string &label, glm::vec2 &values, float resetValue, float columnWidth) {
        ImGui::PushID(label.c_str());
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, 100);
        ImGui::Text(label.c_str());
        ImGui::NextColumn();

        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.5f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

        float lineHeight =
            GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImVec2 buttonSize = {lineHeight + 2.0f, lineHeight};

        // disable button rounded corners
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                              ImVec4{0.9f, 0.2f, 0.2f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                              ImVec4{0.8f, 0.1f, 0.15f, 1.0f});

        if (ImGui::Button("X", buttonSize)) {
            values.x = resetValue;
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                              ImVec4{0.3f, 0.8f, 0.3f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                              ImVec4{0.2f, 0.7f, 0.2f, 1.0f});

        if (ImGui::Button("Y", buttonSize)) {
            values.y = resetValue;
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::Columns(1);
        ImGui::PopStyleVar(3);
        ImGui::PopID();
    }

} // namespace Editor