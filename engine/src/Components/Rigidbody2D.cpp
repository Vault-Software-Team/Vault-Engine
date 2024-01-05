#include <Engine/Components/Rigidbody2D.hpp>
#include <Editor/GUI/MainGUI.hpp>
#include <icons/icons.h>

namespace Engine::Components {
    void Rigidbody2D::Init() {
        transform = &Scene::Main->EntityRegistry.get<Transform>(entity);
    }

    void Rigidbody2D::OnGUI() {
        if (ImGui::TreeNode("Rigidbody 2D")) {
            const char *type_strings[] = {"Static", "Kinematic", "Dynamic"};
            const char *curr_type = type_strings[(int)body_type];
            if (ImGui::BeginCombo("Body Type", curr_type)) {
                for (int i = 0; i < 3; i++) {
                    bool selected = curr_type == type_strings[i];
                    if (ImGui::Selectable(type_strings[i], selected)) {
                        curr_type = type_strings[i];
                        body_type = (BodyType)i;
                    }
                }

                ImGui::EndCombo();
            }
            ImGui::DragFloat("Gravity Scale", &gravity_scale);

            ImGui::TreePop();
        }
    }
} // namespace Engine::Components
  // namespace Engine::Components