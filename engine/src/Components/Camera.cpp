#include <Engine/Components/Camera.hpp>
#include <Engine/Scene.hpp>
#include <Renderer/Window.hpp>
#include <iostream>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Engine::Components {
    void Camera::UpdateMatrix() {
        view = glm::lookAt(transform->position, transform->position + transform->rotation, up);
        projection = glm::perspective(glm::radians(fov), (float)width / height, near, far);
    }

    void Camera::Init() {
        transform = &Scene::Main->EntityRegistry.get<Transform>(entity);
    }

    void Camera::BindToShader(VaultRenderer::Shader &shader) {
        shader.Bind();
        shader.SetUniformMat4("camera_view", view);
        shader.SetUniformMat4("camera_projection", projection);
        shader.SetUniform3f("camera_position", transform->position.x, transform->position.y, transform->position.z);
    }

    void Camera::Inputs() {
        GLFWwindow *window = VaultRenderer::Window::window->GetGLFWWindow();

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            transform->position += speed * transform->rotation;
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            transform->position += speed * -glm::normalize(glm::cross(transform->rotation, up));
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            transform->position += speed * -transform->rotation;
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            transform->position += speed * glm::normalize(glm::cross(transform->rotation, up));
        }

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            transform->position += speed * up;
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            transform->position += speed * -up;
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            speed = 0.4f;
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
            speed = 0.1f;
        }

        // Mouse Input
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

            if (first_click) {
                glfwSetCursorPos(window, (float)width / 2, (float)height / 2);
                first_click = false;
            }

            double mouse_x;
            double mouse_y;

            glfwGetCursorPos(window, &mouse_x, &mouse_y);

            float rotation_x = sensitivity * (float)(mouse_y - ((float)height / 2)) / height;
            float rotation_y = sensitivity * (float)(mouse_x - ((float)width / 2)) / width;

            glm::vec3 new_rotation = glm::rotate(transform->rotation, glm::radians(-rotation_x), glm::normalize(glm::cross(transform->rotation, up)));

            if (!((glm::angle(new_rotation, up) <= glm::radians(5.0f)) || (glm::angle(new_rotation, -up) <= glm::radians(5.0f)))) {
                transform->rotation = new_rotation;
            }
            transform->rotation = glm::rotate(transform->rotation, glm::radians(-rotation_y), up);

            glfwSetCursorPos(window, (float)width / 2, (float)height / 2);
        }
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            first_click = true;
        }
    }
} // namespace Engine::Components