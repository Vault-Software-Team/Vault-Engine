#include <Engine/Components/SpotLight.hpp>
#include <Engine/Scene.hpp>

namespace Engine::Components {
    void SpotLight::Init() {
        transform = &Scene::Main->EntityRegistry.get<Transform>(entity);
    }

    void SpotLight::AttachToShader(VaultRenderer::Shader &shader) {
        int index = 0;
        for (auto e : Scene::Main->EntityRegistry.view<SpotLight>()) {
            if (e == entity)
                break;

            index++;
        }
        this->index = index;

        shader.Bind();
        shader.SetUniform3f(("spot_lights[" + std::to_string(index) + "].position").c_str(), transform->position.x, transform->position.y, transform->position.z);
        shader.SetUniform3f(("spot_lights[" + std::to_string(index) + "].angle").c_str(), transform->rotation.x, transform->rotation.y, transform->rotation.z);
        shader.SetUniform3f(("spot_lights[" + std::to_string(index) + "].color").c_str(), color.x, color.y, color.z);
        shader.SetUniform1f(("spot_lights[" + std::to_string(index) + "].intensity").c_str(), intensity);
    }
} // namespace Engine::Components