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
#include <vector>
#include <memory>

namespace VaultRenderer {
    class Framebuffer {
    public:
        struct ColorAttachement {
            uint32_t ID;
            uint32_t attachement;
        };

        std::vector<std::unique_ptr<ColorAttachement>> color_attachements;

        int width, height;

        uint32_t FBO;
        uint32_t texture, bloomTexture;
        uint32_t RBO;

        uint32_t rectVAO, rectVBO;

        Framebuffer();

        void GenerateFramebuffer();
        void DeleteFramebuffer();
        void RegenerateFramebuffer();
        void AddColorAttachement(uint32_t attachement);
        void Bind();
        void Unbind();
        void UnbindAndDrawOnScreen(Shader &shader);
        uint32_t &GetAttachement(uint32_t attachement);

    private:
        int color_attachements_to_generate;
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
        // glGenTextures(1, &bloomTexture);
        // glBindTexture(GL_TEXTURE_2D, bloomTexture);
        // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0,
        //              GL_RGB, GL_FLOAT, nullptr);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
        //                 GL_LINEAR);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
        //                 GL_LINEAR);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
        //                 GL_CLAMP_TO_EDGE);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
        //                 GL_CLAMP_TO_EDGE);
        // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
        //                        GL_TEXTURE_2D, bloomTexture, 0);
    };
} // namespace VaultRenderer