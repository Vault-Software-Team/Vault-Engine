#pragma once
#include "Engine/Components/Camera.hpp"
#include <dllapi.hpp>
#include <string>
#include <entt/entt.hpp>
#include <Engine/Scene.hpp>
#include <iostream>
#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
    #define GL_GLEXT_PROTOTYPES
    #define EGL_EGLEXT_PROTOTYPES
#else
    #include <glad/glad.h>
#endif
#include <GLFW/glfw3.h>
#include <Renderer/Shader.hpp>

namespace Engine {
    class DLL_API CascadedShadowMap {
    public:
        const int res;
        uint32_t m_FBO;
        uint32_t m_LightDepthMaps;
        uint32_t m_MatricesUBO;

        std::vector<float> m_ShadowCascadeLevels = {};
        std::unique_ptr<VaultRenderer::Shader> shader;

        glm::vec3 *lightDir;

        float farPlane;

        CascadedShadowMap(const int depthMapRes, float cameraFarPlane);

        void BindMap(uint32_t slot);

        void BindForDrawing();
        void UnbindDrawing();

        void BindToShader(VaultRenderer::Shader &shader);

        glm::mat4 GetLightSpaceMatrix(Components::Camera &camera, float near, float far, glm::vec3 lightDir);
        std::vector<glm::mat4> GetLightSpaceMatrices(Components::Camera &camera, glm::vec3 lightDir);

    private:
        void GenerateFBO();
    };
} // namespace Engine