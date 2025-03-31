#pragma once
#include <dllapi.hpp>
#include <entt/entt.hpp>
#include "Base.hpp"
#include <glm/ext.hpp>
#include <Renderer/Shader.hpp>
#include "Transform.hpp"

namespace Engine {
    namespace Components {
        struct DLL_API PointLight : Base {
            static inline const std::string display_name = "Point Light";
            uint32_t shadowMapFBO;
            uint32_t cubemap;
            Transform *transform;

            PointLight() = default;
            void Init();
            glm::vec3 color = glm::vec3(1, 1, 1);
            float intensity = 1.0f;
            float shadow_far_plane = 100.0f;
            bool enable_shadow_mapping = false;
            float ShadowStrength = 1;
            int index = 0;

            void AttachToShader(VaultRenderer::Shader &shader);
            void DrawToShadowMap(VaultRenderer::Shader &shader);
            void OnGUI() override;
        };
    } // namespace Components
} // namespace Engine