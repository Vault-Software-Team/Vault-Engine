#pragma once
#include <cfloat>
#include <dllapi.hpp>
#include "Transform.hpp"
#include "Base.hpp"
#include "glm/ext/scalar_common.hpp"
#include <Renderer/Shader.hpp>

namespace Engine {
    namespace Components {
        struct OrthographicProjectionInfo {
            float r;
            float l;
            float b;
            float t;
            float n;
            float f;
        };

        class AABB {
        public:
            void Add(const glm::vec3 &vec);

            void UpdateOrtho(OrthographicProjectionInfo &ortho);

            float m_MinX = FLT_MAX;
            float m_MaxX = FLT_MIN;
            float m_MinY = FLT_MAX;
            float m_MaxY = FLT_MIN;
            float m_MinZ = FLT_MAX;
            float m_MaxZ = FLT_MIN;
        };

        struct DLL_API Camera : Base {
        public:
            static inline const std::string display_name = "Camera";

            bool first_click;
            Transform *transform;
            void Init();
            Camera() = default;
            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
            glm::vec3 right = glm::vec3(0);

            int width, height;

            float speed = 0.1f;
            float sensitivity = 100.0f;

            glm::mat4 view, projection;
            glm::vec3 front;
            glm::vec3 mouse_rot;

            // Camera Properties
            float fov = 45, near = 0.1, far = 100.0;
            bool main_camera = false;
            bool is2D = false;
            bool depth_camera = false;

            void UpdateMatrix();
            void BindToShader(VaultRenderer::Shader &shader);
            void Inputs(float deltaTime);
            void OnGUI() override;
        };

        class Frustrum {
        public:
            glm::vec4 m_NearTopLeft;
            glm::vec4 m_NearBottomLeft;
            glm::vec4 m_NearTopRight;
            glm::vec4 m_NearBottomRight;

            glm::vec4 m_FarTopLeft;
            glm::vec4 m_FarBottomLeft;
            glm::vec4 m_FarTopRight;
            glm::vec4 m_FarBottomRight;

            void CalculateCorners(const Camera &camera);
            void Transform(const glm::mat4 &model);

            void AABBCalculation(AABB &aabb);
        };

        void
        CalculateTightLightProj(const glm::mat4 &cam_view, const glm::vec3 &light_dir, const Camera &camera, glm::vec3 &light_pos, OrthographicProjectionInfo &ortho_proj);
    } // namespace Components
} // namespace Engine