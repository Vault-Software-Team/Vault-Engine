#include "imgui/imgui.h"
#include <Engine/Components/Camera.hpp>
#include <Engine/Scene.hpp>
#include <Renderer/Window.hpp>
#include <iostream>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Editor/GUI/MainGUI.hpp>
#include <icons/icons.h>

namespace Engine::Components {
    void Camera::UpdateMatrix() {
        glm::vec3 full_rot = transform->rotation;
        view = glm::lookAt(transform->position, transform->position + full_rot, up);
        const float aspect = (float)VaultRenderer::Window::window->targetWidth / VaultRenderer::Window::window->targetHeight;
        if (!is2D) {
            // Isn't 2D
            projection = glm::perspective(glm::radians(fov), aspect, near, far);
            projection = glm::scale(projection, glm::vec3(transform->scale.x, transform->scale.y, 1.0f));
        } else {
            // Is 2D
            projection = glm::ortho(-aspect, aspect, -1.0f, 1.0f, near, far);
            projection = glm::scale(projection, glm::vec3(transform->scale.x, transform->scale.y, 1.0f));
        }

        front = glm::rotate(glm::inverse(glm::quat(full_rot)), glm::vec3(0.0, 0.0, -1.0));
        // glm::lookAt(cam_pos + lightPos, cam_pos + Scene::mainCamera->Front, lightUpThing);
    }

    void Camera::Init() {
        transform = static_registry ? &Scene::StaticGameObjects_EntityRegistry.get<Transform>(entity) : &Scene::Main->EntityRegistry.get<Transform>(entity);
        transform->rotation.z = -1;
    }

    void Camera::BindToShader(VaultRenderer::Shader &shader) {
        shader.Bind();
        shader.SetUniformMat4("camera_view", view);
        shader.SetUniformMat4("camera_projection", projection);
        shader.SetUniform3f("camera_position", transform->position.x, transform->position.y, transform->position.z);
    }

    void Camera::Inputs() {
        GLFWwindow *window = VaultRenderer::Window::window->GetGLFWWindow();
        int width, heigth;
        glfwGetWindowSize(window, &width, &height);
        // width = VaultRenderer::Window::window->width;
        // height = VaultRenderer::Window::window->height;

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
        if (is2D) return;

        int old_width = 0, old_height = 0;

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
            ImGui::SetMouseCursor(ImGuiMouseCursor_None);

            if (first_click) {
                glfwSetCursorPos(window, (float)width / 2, (float)height / 2);
                first_click = false;
                return;
            }

            // have to do thisf or some reason
            // if (old_width - width <= 2 && old_height - height > 0) width = old_width;
            // if (old_height - height <= 2 && old_height - height > 0) height = old_height;
            // if (old_width == width - 1 || old_width == width + 1) width = old_width;
            // if (old_height == height - 1 || old_height == height + 1) height = old_height;

            double mouse_x;
            double mouse_y;

            glfwGetCursorPos(window, &mouse_x, &mouse_y);

            float rotation_x = sensitivity * (mouse_y - (height / 2)) / height;
            float rotation_y = sensitivity * (mouse_x - (width / 2)) / width;

            glm::vec3 new_rotation = glm::rotate(transform->rotation, glm::radians(-rotation_x), glm::normalize(glm::cross(transform->rotation, up)));

            if (!((glm::angle(new_rotation, up) <= glm::radians(5.0f)) || (glm::angle(new_rotation, -up) <= glm::radians(5.0f)))) {
                transform->rotation = new_rotation;
            }
            transform->rotation = glm::rotate(transform->rotation, glm::radians(-rotation_y), up);

            glfwSetCursorPos(window, (float)width / 2, (float)height / 2);

            old_width = width;
            old_height = heigth;
        } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
            first_click = true;
        }
    }

    void Camera::OnGUI() {
        DrawComponent<Camera>(Scene::Main->EntityRegistry, 40, [&] {
            ImGui::DragFloat("FOV", &fov, 0.1f, 0.1f);
            ImGui::DragFloat("Near", &near, 0.1f, 0.0f);
            ImGui::DragFloat("Far", &far, 0.1f, 0.0f);
            if (!depth_camera) {
                ImGui::Checkbox("Main Camera", &main_camera);
            } else {
                main_camera = false;
            }
            ImGui::Checkbox("2D", &is2D);
            ImGui::Checkbox("Overlay", &depth_camera);

            // if (ImGui::Button(check_scene_cam ? "Stop viewing as Scene Camera" : "View as Scene Camera"))

            if (Scene::Main->main_camera_object == this) {
                if (ImGui::Button(ICON_FA_CAMERA " Unselect as Scene Camera")) {
                    Scene::Main->main_camera_object = Scene::Main->EditorSceneCamera;
                }
            } else {
                if (ImGui::Button(ICON_FA_CAMERA " Select as Scene Camera")) {
                    Scene::Main->main_camera_object = this;
                }
            }
        });
    }
} // namespace Engine::Components