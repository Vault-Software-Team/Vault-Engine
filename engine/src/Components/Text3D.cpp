#include <Engine/Components/Text3D.hpp>
#include <Engine/Scene.hpp>
#include <iostream>
#include <Editor/GUI/MainGUI.hpp>
#include <icons/icons.h>
#include <imgui/imgui_stdlib.h>

namespace Engine::Components {
    void Text3D::Draw(VaultRenderer::Shader &shader) {
        transform->UpdateModel();
        font->Draw(shader, transform->model, text, color, glm::vec3(0, 0, 0), 0, 0, scale, y_offset);
    }

    void Text3D::ChangeFont(const std::string &font_path) {
        font.reset();
        font = std::make_unique<VaultRenderer::Font>(font_path.c_str(), 64);
    }

    void Text3D::Init() {
        transform = &Scene::Main->EntityRegistry.get<Transform>(entity);
    }

    void Text3D::OnGUI() {
        if (ImGui::TreeNode("Text 3D")) {
            ImGui::InputTextMultiline("Text", &text);
            ImGui::DragFloat("Scale", &scale, 0.001f);
            ImGui::DragFloat("Y Offset", &y_offset, 0.01f);
            ImGui::ColorEdit3("Color", &color.x);
            // ImGui::ColorEdit3("Color", &color.x);

            ImVec2 winSize = ImGui::GetWindowSize();
            if (ImGui::Button(ICON_FA_TRASH " Delete", ImVec2(winSize.x - 50, 0))) {
                GameObject::FindGameObjectByEntity(entity)->RemoveComponent<Text3D>();
            }

            ImGui::TreePop();
        }
    }
} // namespace Engine::Components