#pragma once
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

namespace VaultRenderer {
    class Framebuffer {
    public:
        uint32_t FBO;
        uint32_t texture;
        uint32_t RBO;

        uint32_t rectVAO, rectVBO;

        Framebuffer();

        void GenerateFramebuffer();
        void DeleteFramebuffer();
        void RegenerateFramebuffer();
        void Bind();
        void Unbind();
        void UnbindAndDrawOnScreen(Shader &shader);
    };

    class DepthFramebuffer {
    public:
        uint32_t FBO;
        uint32_t width = 2048, height = 2048;
        uint32_t texture;

        DepthFramebuffer();
        void GenerateFramebuffer();
        void DeleteFramebuffer();
        void RegenerateFramebuffer();
        void Bind();
        void Unbind();
        void BindTexture(uint32_t slot = 0);
    };
} // namespace VaultRenderer