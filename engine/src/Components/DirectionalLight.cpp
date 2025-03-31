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

        if (enable_shadow_mapping) {
            shader.SetUniform1f("dirshadow_bias_max", bias_max);
            shader.SetUniform1f("dirshadow_bias_min", bias_min);
        }
    }

    void DirectionalLight::OnGUI() {
        DrawComponent<DirectionalLight>(Scene::Main->EntityRegistry, 40, [&] {
            ImGui::DragFloat("Intensity", &intensity, 0.01f, 0.0);
            ImGui::ColorEdit3("Color", &color.x, ImGuiColorEditFlags_HDR);
            ImGui::Checkbox("Shadows", &enable_shadow_mapping);
            ImGui::DragFloat("Max Shadow Bias", &bias_max, 0.001f, 0.0);
            ImGui::DragFloat("Min Shadow Bias", &bias_min, 0.0001f, 0.0);
        });
    }
} // namespace Engine::Components