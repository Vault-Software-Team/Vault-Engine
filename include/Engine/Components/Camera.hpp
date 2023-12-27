#pragma once
#include <dllapi.hpp>
#include "Transform.hpp"
#include "Base.hpp"
#include <Renderer/Shader.hpp>

namespace Engine {
    namespace Components {
        struct DLL_API Camera : Base {
        private:
            bool first_click;

        public:
            Transform *transform;
            void Init();
            Camera() = default;
            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

            int width, height;

            float speed = 0.1f;
            float sensitivity = 100.0f;

            glm::mat4 view, projection;

            // Camera Properties
            float fov, near, far;
            bool main_camera;

            void UpdateMatrix();
            void BindToShader(VaultRenderer::Shader &shader);
            void Inputs();
            void OnGUI() override;
        };
    } // namespace Components
} // namespace Engine