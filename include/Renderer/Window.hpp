#pragma once
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <functional>
#include <dllapi.hpp>
#include <Renderer/Framebuffer.hpp>
#include <memory>
#include <Renderer/Shader.hpp>

namespace VaultRenderer {
    class DLL_API Window {
    public:
        static inline Window *window;
        std::unique_ptr<Framebuffer> framebuffer = nullptr;

        Window(const int width, const int height, const char *title);
        ~Window();

        int width;
        int height;
        std::string title;

        GLFWwindow *GetGLFWWindow() const;

        void Run(std::function<void()> update_call);
        void SetClearColor(const float r, const float g, const float b);
        void SetClearColor(const uint32_t hex_color);
        void SetViewport(const int width, const int height);

    private:
        static void FramebufferSizeCallback(GLFWwindow *window, int width, int height);
        void SetupImGui();
        void SetDefaultImGuiTheme();
        mutable GLFWwindow *glfw_window;
    };
} // namespace VaultRenderer