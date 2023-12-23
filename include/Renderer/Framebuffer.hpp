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
} // namespace VaultRenderer