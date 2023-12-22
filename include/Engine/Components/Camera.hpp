#pragma once
#include "Transform.hpp"
#include "Base.hpp"
#include <Renderer/Shader.hpp>

namespace Engine {
    namespace Components {
        struct Camera : Base {
            Camera() = default;
            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

            int width, height;

            float speed = 0.1f;
            float sensitivity = 100.0f;

            glm::mat4 view, projection;

            // Camera Properties
            float fov, near, far;

            void UpdateMatrix();
            void BindToShader(VaultRenderer::Shader &shader);
        };
    } // namespace Components
} // namespace Engine