#include "glm/ext/matrix_transform.hpp"
#include "glm/matrix.hpp"
#include "glm/trigonometric.hpp"
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
    void AABB::Add(const glm::vec3 &vec) {
        m_MinX = glm::min(m_MinX, vec.x);
        m_MinY = glm::min(m_MinY, vec.y);
        m_MinZ = glm::min(m_MinZ, vec.z);

        m_MaxX = glm::max(m_MaxX, vec.x);
        m_MaxY = glm::max(m_MaxY, vec.y);
        m_MaxZ = glm::max(m_MaxZ, vec.z);
    }

    void AABB::UpdateOrtho(OrthographicProjectionInfo &ortho) {
        ortho.r = m_MaxX;
        ortho.l = m_MinX;

        ortho.b = m_MinY;
        ortho.t = m_MaxY;

        ortho.n = m_MinZ;
        ortho.f = m_MaxZ;
    }

    void Frustrum::CalculateCorners(const Camera &camera) {
        const float aspect_ratio = (float)VaultRenderer::Window::window->targetWidth / VaultRenderer::Window::window->targetHeight;
        float tan_half_fov = tanf(glm::radians(camera.fov / 2.f));

        float nearZ = camera.near;
        float nearX = nearZ * tan_half_fov;
        float nearY = nearZ * tan_half_fov * aspect_ratio;

        m_NearTopLeft = glm::vec4(-nearX, nearY, nearZ, 1.f);
        m_NearBottomLeft = glm::vec4(-nearX, -nearY, nearZ, 1.f);
        m_NearTopRight = glm::vec4(nearX, nearY, nearZ, 1.f);
        m_NearBottomRight = glm::vec4(nearX, -nearY, nearZ, 1.f);

        float farZ = camera.far;
        float farX = farZ * tan_half_fov;
        float farY = farZ * tan_half_fov * aspect_ratio;

        m_FarTopLeft = glm::vec4(-farX, farY, farZ, 1.f);
        m_FarBottomLeft = glm::vec4(-farX, -farY, farZ, 1.f);
        m_FarTopRight = glm::vec4(farX, farY, farZ, 1.f);
        m_FarBottomRight = glm::vec4(farX, -farY, farZ, 1.f);
    }

    void Frustrum::Transform(const glm::mat4 &model) {
        m_NearTopLeft = model * m_NearTopLeft;
        m_NearBottomLeft = model * m_NearBottomLeft;
        m_NearTopRight = model * m_NearTopRight;
        m_NearBottomRight = model * m_NearBottomRight;

        m_FarTopLeft = model * m_FarTopLeft;
        m_FarBottomLeft = model * m_FarBottomLeft;
        m_FarTopRight = model * m_FarTopRight;
        m_FarBottomRight = model * m_FarBottomRight;
    }

    void Frustrum::AABBCalculation(AABB &aabb) {
        aabb.Add(m_NearTopLeft);
        aabb.Add(m_NearBottomLeft);
        aabb.Add(m_NearTopRight);
        aabb.Add(m_NearBottomRight);

        aabb.Add(m_FarTopLeft);
        aabb.Add(m_FarBottomLeft);
        aabb.Add(m_FarTopRight);
        aabb.Add(m_FarBottomRight);
    }

    void CalculateTightLightProj(const glm::mat4 &cam_view, const glm::vec3 &light_dir, const Camera &camera, glm::vec3 &light_pos, OrthographicProjectionInfo &ortho_proj) {
        Frustrum frustrum;
        frustrum.CalculateCorners(camera);

        glm::mat4 inv_cam_view = glm::inverse(cam_view);
        frustrum.Transform(inv_cam_view);

        Frustrum view_frustrim_world_space = frustrum;

        glm::vec3 origin(0.f, 0.f, 0.f);
        glm::vec3 up(0.f, 1.f, 0.f);

        glm::mat4 light_view = glm::lookAt(origin, light_dir, glm::vec3(0, 1, 0));
        frustrum.Transform(light_view);

        AABB aabb;
        frustrum.AABBCalculation(aabb);

        glm::vec3 bottom_left(aabb.m_MinX, aabb.m_MinY, aabb.m_MinZ);
        glm::vec3 top_right(aabb.m_MaxX, aabb.m_MaxY, aabb.m_MaxZ);
        glm::vec4 light_pos_world_4d = glm::vec4((bottom_left + top_right) / 2.f, 1.f);

        glm::mat4 light_view_inv = glm::inverse(light_view);
        light_pos_world_4d = light_view_inv * light_pos_world_4d;
        light_pos = glm::vec3(light_pos_world_4d.x, light_pos_world_4d.y, light_pos_world_4d.z);

        light_view = glm::lookAt(light_pos, light_dir, up);
        view_frustrim_world_space.Transform(light_view);

        AABB final;
        view_frustrim_world_space.AABBCalculation(final);

        final.UpdateOrtho(ortho_proj);
    }

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
        right = view * projection * glm::vec4(1.0, 0.0, 0.0, 0.0);
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

    void Camera::Inputs(float deltaTime) {
        GLFWwindow *window = VaultRenderer::Window::window->GetGLFWWindow();
        int width, heigth;
        glfwGetWindowSize(window, &width, &height);
        // width = VaultRenderer::Window::window->width;
        // height = VaultRenderer::Window::window->height;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            transform->position += speed * transform->rotation * deltaTime;
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            transform->position += speed * -glm::normalize(glm::cross(transform->rotation, up)) * deltaTime;
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            transform->position += speed * -transform->rotation * deltaTime;
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            transform->position += speed * glm::normalize(glm::cross(transform->rotation, up)) * deltaTime;
        }

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            transform->position += speed * up * deltaTime;
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            transform->position += speed * -up * deltaTime;
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            speed = 25.f;
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
            speed = 10.f;
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