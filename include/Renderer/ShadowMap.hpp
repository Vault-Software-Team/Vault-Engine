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
#include <Renderer/Framebuffer.hpp>
#include <Renderer/Shader.hpp>
#include <functional>
#include <memory>

namespace VaultRenderer {
    class DLL_API ShadowMap {
    private:
        glm::mat4 light_proj;
        glm::mat4 light_view;
        glm::mat4 ortho_proj;
        DepthFramebuffer depth_buffer;

    public:
        float ortho_size = 35.0;
        float near = 1.0;
        float far = 75.0;
        std::unique_ptr<Shader> shader;

        ShadowMap();

        void RenderSpace(std::function<void(std::unique_ptr<Shader> &)> shadow_render_call);
        void ChangeShadowMapSize(const uint32_t width, const uint32_t height);
        void BindTexture(int slot = 10);
        void CalculateMatrices(const glm::vec3 &light_position);
        DepthFramebuffer &GetDepthBuffer();
        void SetLightProjection(Shader &shader);
        uint32_t &GetTextureID();
    };
} // namespace VaultRenderer