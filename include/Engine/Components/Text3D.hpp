#pragma once
#include <dllapi.hpp>
#include <entt/entt.hpp>
#include "Base.hpp"
#include <glm/ext.hpp>
#include <Renderer/Shader.hpp>
#include <Renderer/Font.hpp>
#include "Transform.hpp"
#include <memory>

namespace Engine {
    namespace Components {
        struct DLL_API Text3D : Base {
            std::unique_ptr<VaultRenderer::Font> font = nullptr;
            Transform *transform = nullptr;

            Text3D() = default;
            void Init();
            glm::vec3 color = glm::vec3(1, 1, 1);
            std::string text = "Hello, World!";
            float y_offset = 1;
            float scale = 0.020;

            void ChangeFont(const std::string &font_path);

            void Draw(VaultRenderer::Shader &shader);
        };
    } // namespace Components
} // namespace Engine