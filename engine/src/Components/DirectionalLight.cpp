#include <Engine/Components/DirectionalLight.hpp>
#include <Engine/Scene.hpp>
#include <Editor/GUI/MainGUI.hpp>
#include <icons/icons.h>

namespace Engine::Components {
    void DirectionalLight::Init() {
        transform = &Scene::Main->EntityRegistry.get<Transform>(entity);
    }

    void DirectionalLight::AttachToShader(VaultRenderer::Shader &shader) {
        int index = 0;
        for (auto e : Scene::Main->EntityRegistry.view<DirectionalLight>()) {
            if (e == entity)
                break;

            index++;
        }
        this->index = index;

        shader.Bind();
        shader.SetUniform3f(("directional_lights[" + std::to_string(index) + "].position").c_str(), transform->position.x, transform->position.y, transform->position.z);
        shader.SetUniform3f(("directional_lights[" + std::to_string(index) + "].color").c_str(), color.x, color.y, color.z);
        shader.SetUniform1f(("directional_lights[" + std::to_string(index) + "].intensity").c_str(), intensity);
    }

    void DirectionalLight::OnGUI() {
        if (ImGui::TreeNode("Directional Light")) {
            ImGui::DragFloat("Intensity", &intensity, 0.01f, 0.0);
            ImGui::Checkbox("Shadows", &enable_shadow_mapping);
            ImGui::ColorEdit3("Color", &color.x);

            // float intensity = 1.0f;
            // float shadow_far_plane = 100.0f;
            // bool enable_shadow_mapping = false

            ImVec2 winSize = ImGui::GetWindowSize();
            if (ImGui::Button(ICON_FA_TRASH " Delete", ImVec2(winSize.x - 50, 0))) {
                GameObject::FindGameObjectByEntity(entity)->RemoveComponent<DirectionalLight>();
            }

            ImGui::TreePop();
        }
    }
} // namespace Engine::Components