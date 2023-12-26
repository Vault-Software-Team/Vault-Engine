#include <Renderer/Framebuffer.hpp>
#include <Renderer/Window.hpp>
#include <iostream>

float rectangleVertices[] =
    {
        // Coords    // texCoords
        1.0f, -1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
        -1.0f, 1.0f, 0.0f, 1.0f,

        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f, 1.0f};

namespace VaultRenderer {
    Framebuffer::Framebuffer(bool have_housing) {
        if (have_housing)
            framebuffer = std::make_unique<Framebuffer>();

        // Framebuffer
        GenerateFramebuffer();

        // Rectangle
        glGenVertexArrays(1, &rectVAO);
        glGenBuffers(1, &rectVBO);
        glBindVertexArray(rectVAO);
        glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), &rectangleVertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    }

    void Framebuffer::GenerateFramebuffer() {
        glGenFramebuffers(1, &FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);

        // Texture
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Window::window->width, Window::window->height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

        // Attachements
        for (auto &a : color_attachements) {
            glGenTextures(1, &a.ID);
            glBindTexture(GL_TEXTURE_2D, a.ID);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + a.attachement, GL_TEXTURE_2D, a.ID, 0);
        }

        // RBO
        glGenRenderbuffers(1, &RBO);
        glBindRenderbuffer(GL_RENDERBUFFER, RBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Window::window->width, Window::window->height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

        auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "FB Error: " << fboStatus << "\n";

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::DeleteFramebuffer() {
        glDeleteFramebuffers(1, &FBO);
        glDeleteTextures(1, &texture);
        glDeleteRenderbuffers(1, &RBO);

        for (auto &a : color_attachements) {
            glDeleteTextures(1, &a.ID);
        }
    }

    void Framebuffer::RegenerateFramebuffer() {
        DeleteFramebuffer();
        if (framebuffer)
            framebuffer->DeleteFramebuffer();
        GenerateFramebuffer();
        if (framebuffer)
            framebuffer->GenerateFramebuffer();
    }

    void Framebuffer::Bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    }

    void Framebuffer::Unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::UnbindAndDrawOnScreen(Shader &shader) {
        framebuffer->width = width;
        framebuffer->height = height;

        if (draw_screen) {
            glViewport(0, 0, VaultRenderer::Window::window->width, VaultRenderer::Window::window->height);
            glDisable(GL_CULL_FACE);
            shader.Bind();
            Unbind();

            glBindVertexArray(rectVAO);
            glDisable(GL_DEPTH_TEST);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);
            shader.SetUniform1i("screen_texture", 0);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glEnable(GL_DEPTH_TEST);
        } else {
            // Bind a framebuffer that houses our actual framebuffer, we do this so that the framebuffer shader gets applied to the texture wink wink
            framebuffer->Bind();
            // Clear the framebuffer
            glClearColor(0, 0, 0, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glViewport(0, 0, VaultRenderer::Window::window->width, VaultRenderer::Window::window->height);
            // Enable Depth
            glEnable(GL_DEPTH_TEST);

            // Disable cull face and bind the framebuffer shader provided
            glDisable(GL_CULL_FACE);
            shader.Bind();

            // Draw the framebuffer to a quad
            glBindVertexArray(rectVAO);
            glDisable(GL_DEPTH_TEST);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);
            shader.SetUniform1i("screen_texture", 0);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glEnable(GL_DEPTH_TEST);

            // Unbind the static framebuffer that houses the actual framebuffer
            framebuffer->Unbind();
        }

        Unbind();
        // glEnable(GL_CULL_FACE);
        glViewport(0, 0, VaultRenderer::Window::window->width, VaultRenderer::Window::window->height);
    }

    void Framebuffer::AddColorAttachement(uint32_t attachement) {
        Bind();
        color_attachements.push_back({0, attachement});
        glGenTextures(1, &color_attachements.back().ID);
        glBindTexture(GL_TEXTURE_2D, color_attachements.back().ID);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachement, GL_TEXTURE_2D, color_attachements.back().ID, 0);
    }

    void Framebuffer::BindAttachement(const uint32_t attachement, const uint32_t &slot) {
        uint32_t &ID = GetAttachement(attachement);
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, ID);
    }

    uint32_t &Framebuffer::GetAttachement(uint32_t attachement) {
        for (auto &a : color_attachements) {
            if (a.attachement == attachement)
                return a.ID;
        }
    }

    DepthFramebuffer::DepthFramebuffer() {
        GenerateFramebuffer();
    }

    void DepthFramebuffer::GenerateFramebuffer() {
        glGenFramebuffers(1, &FBO);

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float clampColor[] = {1, 1, 1, 1};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor);

        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void DepthFramebuffer::DeleteFramebuffer() {
        glDeleteFramebuffers(1, &FBO);
        glDeleteTextures(1, &texture);
    }

    void DepthFramebuffer::RegenerateFramebuffer() {
        DeleteFramebuffer();
        GenerateFramebuffer();
    }

    void DepthFramebuffer::Bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    }

    void DepthFramebuffer::Unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void DepthFramebuffer::BindTexture(uint32_t slot) {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, texture);
    }

} // namespace VaultRenderer