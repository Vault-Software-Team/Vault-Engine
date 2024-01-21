#pragma once
#include <dllapi.hpp>
#include "Transform.hpp"
#include "Base.hpp"
#include <Renderer/Shader.hpp>

namespace Engine {
    namespace Components {
        struct DLL_API Camera : Base {
        public:
            static inline const std::string display_name = "Camera";

            bool first_click;
            Transform *transform;
            void Init();
            Camera() = default;
            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

            int width, height;

            float speed = 0.1f;
            float sensitivity = 100.0f;

            glm::mat4 view, projection;

            // Camera Properties
            float fov = 45, near = 0.1, far = 100.0;
            bool main_camera = false;
            bool is2D = false;

            void UpdateMatrix();
            void BindToShader(VaultRenderer::Shader &shader);
            void Inputs();
            void OnGUI() override;
        };
    } // namespace Components
} // namespace Engine