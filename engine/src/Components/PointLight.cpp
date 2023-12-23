#include <Engine/Components/PointLight.hpp>
#include <Engine/Scene.hpp>

namespace Engine::Components {
    void PointLight::Init() {
        transform = &Scene::Main->EntityRegistry.get<Transform>(entity);
    }

    void PointLight::AttachToShader(VaultRenderer::Shader &shader) {
        int index = 0;
        for (auto e : Scene::Main->EntityRegistry.view<PointLight>()) {
            if (e == entity)
                break;

            index++;
        }
        this->index = index;

        shader.Bind();
        shader.SetUniform3f(("point_lights[" + std::to_string(index) + "].position").c_str(), transform->position.x, transform->position.y, transform->position.z);
        shader.SetUniform3f(("point_lights[" + std::to_string(index) + "].color").c_str(), color.x, color.y, color.z);
        shader.SetUniform1f(("point_lights[" + std::to_string(index) + "].intensity").c_str(), intensity);
    }
} // namespace Engine::Components