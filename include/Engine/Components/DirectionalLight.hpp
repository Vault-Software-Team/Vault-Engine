#pragma once
#include <dllapi.hpp>
#include <entt/entt.hpp>
#include "Base.hpp"
#include <glm/ext.hpp>
#include <Renderer/Shader.hpp>
#include "Transform.hpp"

namespace Engine {
    namespace Components {
        struct DLL_API DirectionalLight : Base {
            static inline const std::string display_name = "Directional Light";
            Transform *transform;

            DirectionalLight() = default;
            void Init();
            glm::vec3 color = glm::vec3(1, 1, 1);
            float intensity = 1.0f;
            bool enable_shadow_mapping = false;
            int index = 0;

            void AttachToShader(VaultRenderer::Shader &shader);
            void OnGUI() override;
        };
    } // namespace Components
} // namespace Engine