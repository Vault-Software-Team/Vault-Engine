#pragma once
#include <dllapi.hpp>
#include <cstdint>
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
    class DLL_API Skybox {
    public:
        uint32_t skyboxVAO, skyboxVBO, skyboxEBO, cubemapTexture;

        Skybox();
        void Render(Shader &shader, glm::vec3 camPos, glm::vec3 camRot, glm::vec3 camUp);
        void RenderOther(uint32_t texture, Shader &shader, glm::vec3 camPos, glm::vec3 camRot, glm::vec3 camUp);
    };
} // namespace VaultRenderer