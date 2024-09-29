#include "Renderer/Window.hpp"
#include <Engine/Input/Input.hpp>
#include <iostream>

namespace Engine {
    std::unordered_map<int, bool> Input::keys;
    glm::vec2 Input::winPos;

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

    int Input::GetMouseXAxis() {
        double x, y;
        glfwGetCursorPos(VaultRenderer::Window::window->GetGLFWWindow(), &x, &y);

        if (x < winPos.x + (float)VaultRenderer::Window::window->width / 2) {
            return -1;
        } else if (x > winPos.x + (float)VaultRenderer::Window::window->width / 2) {
            return 1;
        }

        return 0;
    }

    int Input::GetMouseYAxis() {
        double x, y;
        glfwGetCursorPos(VaultRenderer::Window::window->GetGLFWWindow(), &x, &y);

        if (y < winPos.y + (float)VaultRenderer::Window::window->height / 2) {
            return -1;
        } else if (y > winPos.y + (float)VaultRenderer::Window::window->height / 2) {
            return 1;
        }

        return 0;
    }

    int Input::GetHorizontalAxis() {
        if (IsKeyPressed(GLFW_KEY_A) || IsKeyPressed(GLFW_KEY_LEFT)) {
            return -1;
        } else if (IsKeyPressed(GLFW_KEY_D) || IsKeyPressed(GLFW_KEY_RIGHT)) {
            return 1;
        }
        return 0;
    }

    int Input::GetVerticalAxis() {
        if (IsKeyPressed(GLFW_KEY_W) || IsKeyPressed(GLFW_KEY_UP)) {
            return 1;
        } else if (IsKeyPressed(GLFW_KEY_S) || IsKeyPressed(GLFW_KEY_DOWN)) {
            return -1;
        }
        return 0;
    }

} // namespace Engine