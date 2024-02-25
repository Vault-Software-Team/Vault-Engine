#pragma once
#include <dllapi.hpp>
#include <entt/entt.hpp>
#include "Base.hpp"
#include <glm/ext.hpp>
#include <Renderer/Mesh.hpp>
#include "Transform.hpp"
#include "glm/fwd.hpp"

namespace Engine {
    namespace Components {
        struct DLL_API SpritesheetRenderer : Base {
            static inline const std::string display_name = "Spritesheet Renderer";
            std::unique_ptr<VaultRenderer::Mesh> mesh;
            Transform *transform;
            glm::vec2 spritesheetSize{512, 512};
            glm::vec2 spriteSize{32, 32};
            glm::vec2 spriteOffset{0, 0};

            SpritesheetRenderer() = default;
            void Init() override;

            void Draw(VaultRenderer::Shader &shader);
            void OnGUI() override;
            void Update() override;
        };
    } // namespace Components
} // namespace Engine