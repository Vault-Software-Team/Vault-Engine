#pragma once
#include <dllapi.hpp>
#include <entt/entt.hpp>
#include "Base.hpp"
#include <glm/ext.hpp>
#include <Renderer/Shader.hpp>
#include "Transform.hpp"

namespace Engine {
    namespace Components {
        struct DLL_API SpotLight : Base {
            Transform *transform;

            SpotLight() = default;
            void Init();
            glm::vec3 color = glm::vec3(1, 1, 1);
            float intensity = 1.0f;
            int index = 0;

            void AttachToShader(VaultRenderer::Shader &shader);
        };
    } // namespace Components
} // namespace Engine