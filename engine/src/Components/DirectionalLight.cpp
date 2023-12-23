#include <Engine/Components/DirectionalLight.hpp>
#include <Engine/Scene.hpp>

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
} // namespace Engine::Components