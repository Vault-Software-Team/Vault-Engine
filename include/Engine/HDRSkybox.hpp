#pragma once
#include "Engine/Components/Camera.hpp"
#include "Renderer/Texture.hpp"
#include <dllapi.hpp>
#include <cstdint>
#include <memory>
#include <string>
#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
    #define GL_GLEXT_PROTOTYPES
    #define EGL_EGLEXT_PROTOTYPES
#else
    #include <glad/glad.h>
#endif
#include <GLFW/glfw3.h>
#include <Renderer/Shader.hpp>
#include <glm/ext.hpp>

namespace VaultRenderer {
    class DLL_API HDRSkybox {
    public:
        uint32_t envCubemap, irradianceMap, prefilterMap, brdfLUT;
        uint32_t captureFBO, captureRBO;
        uint32_t icaptureFBO, icaptureRBO;
        uint32_t pcaptureFBO, pcaptureRBO;
        uint32_t bcaptureFBO, bcaptureRBO;

        std::unique_ptr<Texture> hdr_texture;

        Shader *equirectToCubemapShader, *hdriSkyboxShader, *irrShader, *prefilterShader, *brdfShader;

        HDRSkybox(const std::string &tex, Shader *equirectToCubemapShader, Shader *hdriSkyboxShader, Shader *irrShader, Shader *prefilterShader, Shader *brdfShader, const int res);

        const int res;

        glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
        std::vector<glm::mat4> captureViews = {
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)) //
        };

        // Call this every frame otherwise envCubemap doesnt appear??
        void RenderEnvCubemap();
        void BindEnvCubemap(uint32_t slot);
        void BindIrradianceMap(uint32_t slot);
        void BindPrefilterMap(uint32_t slot);
        void BindBRDFLUT(uint32_t slot);
        void RenderSkybox(Shader &skyboxShader, Engine::Components::Camera &camera);
    };
} // namespace VaultRenderer