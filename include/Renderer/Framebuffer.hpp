#pragma once
#include <cstdint>
#include <string>
#include <glad/glad.h>
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