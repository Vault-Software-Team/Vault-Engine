#include "Renderer/Window.hpp"
#include <Engine/Input/Input.hpp>
#include <iostream>

namespace Engine {
    std::unordered_map<int, bool> Input::keys;

    bool Input::IsKeyPressed(int key) {
        if (glfwGetKey(VaultRenderer::Window::window->GetGLFWWindow(), key) == GLFW_PRESS) {
            return true;
        }

        return false;
    }

    bool Input::IsKeyReleased(int key) {
        if (glfwGetKey(VaultRenderer::Window::window->GetGLFWWindow(), key) == GLFW_RELEASE) {
            return true;
        }

        return false;
    }

    bool Input::IsKeyDown(int key) {
        if (glfwGetKey(VaultRenderer::Window::window->GetGLFWWindow(), key) == GLFW_PRESS && !keys[key]) {
            keys[key] = true;
            return true;
        }

        if (glfwGetKey(VaultRenderer::Window::window->GetGLFWWindow(), key) == GLFW_RELEASE && keys[key]) {
            keys[key] = false;
        }

        return false;
    }

    bool Input::IsKeyUp(int key) {
        if (glfwGetKey(VaultRenderer::Window::window->GetGLFWWindow(), key) == GLFW_PRESS && !keys[key]) {
            keys[key] = true;
        }

        if (glfwGetKey(VaultRenderer::Window::window->GetGLFWWindow(), key) == GLFW_RELEASE && keys[key]) {
            keys[key] = false;
            return true;
        }

        return false;
    }

} // namespace Engine