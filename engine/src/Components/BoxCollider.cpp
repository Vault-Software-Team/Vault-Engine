#include <Engine/Components/BoxCollider2D.hpp>
#include <Editor/GUI/MainGUI.hpp>
#include <icons/icons.h>

namespace Engine::Components {
    void BoxCollider2D::Init() {
        transform = &Scene::Main->EntityRegistry.get<Transform>(entity);
    }

    void BoxCollider2D::OnGUI() {
        if (ImGui::TreeNode("Box Collider 2D")) {
            Editor::GUI::DrawVec2Control("Offset", offset);
            Editor::GUI::DrawVec2Control("Size", size);
            ImGui::DragFloat("Density", &density, 0.01);
            ImGui::DragFloat("Friction", &friction, 0.01);
            ImGui::DragFloat("Restitution", &restitution, 0.01);
            ImGui::Checkbox("Trigger", &trigger);

            ImGui::TreePop();
        }
    }
} // namespace Engine::Components
  // namespace Engine::Components