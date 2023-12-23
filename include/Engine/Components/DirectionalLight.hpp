#pragma once
#include <entt/entt.hpp>
#include "Base.hpp"
#include <dllapi.hpp>
#include <glm/ext.hpp>
#include <Renderer/Shader.hpp>
#include "Transform.hpp"

namespace Engine {
    namespace Components {
        struct DLL_API DirectionalLight : Base {
            Transform *transform;

            DirectionalLight() = default;
            void Init();
            glm::vec3 color = glm::vec3(1, 1, 1);
            float intensity = 1.0f;
            int index = 0;

            void AttachToShader(VaultRenderer::Shader &shader);
        };
    } // namespace Components
} // namespace Engine