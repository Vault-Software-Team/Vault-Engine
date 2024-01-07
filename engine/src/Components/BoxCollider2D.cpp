#include <Engine/Components/BoxCollider2D.hpp>
#include <Editor/GUI/MainGUI.hpp>
#include <icons/icons.h>

namespace Engine::Components {
    void BoxCollider2D::Init() {
        transform = &Scene::Main->EntityRegistry.get<Transform>(entity);
    }

    void BoxCollider2D::OnGUI() {
        DrawComponent<BoxCollider2D>(Scene::Main->EntityRegistry, 40, [&] {
            Editor::GUI::DrawVec2Control("Offset", offset);
            Editor::GUI::DrawVec2Control("Size", size);
            ImGui::DragFloat("Density", &density, 0.01);
            ImGui::DragFloat("Friction", &friction, 0.01);
            ImGui::DragFloat("Restitution", &restitution, 0.01);
            ImGui::Checkbox("Trigger", &trigger);
        });
    }
} // namespace Engine::Components
  // namespace Engine::Components