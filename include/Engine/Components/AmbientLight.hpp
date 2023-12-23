#pragma once
#include <entt/entt.hpp>
#include "Base.hpp"
#include <dllapi.hpp>
#include <glm/ext.hpp>
#include <Renderer/Shader.hpp>

namespace Engine {
    namespace Components {
        struct DLL_API AmbientLight : Base {
            AmbientLight() = default;
            glm::vec3 color = glm::vec3(1, 1, 1);
            float amount = 0.2f;

            void AttachToShader(VaultRenderer::Shader &shader);
        };
    } // namespace Components
} // namespace Engine