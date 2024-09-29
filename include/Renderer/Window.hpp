#pragma once
#include <dllapi.hpp>
#include <string>
#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
    #define GL_GLEXT_PROTOTYPES
    #define EGL_EGLEXT_PROTOTYPES
#else
    #include <glad/glad.h>
#endif
#include <GLFW/glfw3.h>
#include <functional>
#include <Renderer/Framebuffer.hpp>
#include <memory>
#include <Renderer/Shader.hpp>

namespace VaultRenderer {
    struct RendererConfig {
        bool Bloom = true;
    };

    class DLL_API Window {
    public:
        static DLL_API Window *window;
        std::unique_ptr<Framebuffer> framebuffer = nullptr;
        std::unique_ptr<Framebuffer> m_PostProcessingFramebuffer = nullptr;

        bool use_imgui_size = false;

        Window(const int width, const int height, const char *title, bool draw_framebuffer = false);
        ~Window();

        int width;
        int height;
        const int targetWidth = 1920, targetHeight = 1080;
        glm::vec2 mouse_pos{0, 0};
        float gamma = 2.2;
        std::string title;

        GLFWwindow *GetGLFWWindow() const;

        // RENDERER SETTINGS
        static RendererConfig Renderer;

        void Run(std::function<void(Shader &)> update_call, std::function<void()> gui_call, std::function<void()> shadow_render_call, std::function<void(Shader &)> framebuffer_shader_config, std::function<void(Framebuffer::ColorAttachement &)> mouse_picking);
        void SetClearColor(const float r, const float g, const float b);
        void SetClearColor(const uint32_t hex_color);
        void SetViewport(const int width, const int height);
        void AspectRatioCameraViewport();
        void SetMousePos(float x, float y);
        void SetMousePos(glm::vec2 xy);
        void SetWindowIcon(const std::string &path);

    private:
        static DLL_API void FramebufferSizeCallback(GLFWwindow *window, int width, int height);
        void SetupImGui();
        void SetDefaultImGuiTheme();
        mutable GLFWwindow *glfw_window;
    };
} // namespace VaultRenderer