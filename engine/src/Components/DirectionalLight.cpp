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
        DrawComponent<DirectionalLight>(Scene::Main->EntityRegistry, 40, [&] {
            ImGui::DragFloat("Intensity", &intensity, 0.01f, 0.0);
            ImGui::Checkbox("Shadows", &enable_shadow_mapping);
            ImGui::ColorEdit3("Color", &color.x);
        });
    }
} // namespace Engine::Components