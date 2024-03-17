#include <Engine/Components/PointLight.hpp>
#include <Engine/Scene.hpp>
#include <Engine/Components/MeshRenderer.hpp>
#include <Engine/Components/Text3D.hpp>
#include <Renderer/Window.hpp>
#include <Editor/GUI/MainGUI.hpp>
#include <icons/icons.h>

namespace Engine::Components {
    void PointLight::Init() {
        transform = &Scene::Main->EntityRegistry.get<Transform>(entity);

        glGenFramebuffers(1, &shadowMapFBO);
        glGenTextures(1, &cubemap);

        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

        for (uint32_t i = 0; i < 6; i++) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, 2048, 2048, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, cubemap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void PointLight::AttachToShader(VaultRenderer::Shader &shader) {
        int index = 0;
        for (auto e : Scene::Main->EntityRegistry.view<PointLight>()) {
            if (e == entity)
                break;

            index++;
        }
        this->index = index;

        shader.Bind();
        shader.SetUniform3f(("point_lights[" + std::to_string(index) + "].position").c_str(), transform->position.x, transform->position.y, transform->position.z);
        shader.SetUniform3f(("point_lights[" + std::to_string(index) + "].color").c_str(), color.x, color.y, color.z);
        shader.SetUniform1f(("point_lights[" + std::to_string(index) + "].intensity").c_str(), intensity);
    }

    void PointLight::DrawToShadowMap(VaultRenderer::Shader &shader) {
        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, 2046, 2046);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        auto v = Scene::Main->EntityRegistry.view<MeshRenderer>();
        auto v2 = Scene::Main->EntityRegistry.view<Text3D>();

        shader.Bind();

        glm::mat4 shadow_projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, shadow_far_plane);
        glm::mat4 shadow_transforms[] = {
            shadow_projection * glm::lookAt(transform->position, transform->position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0, -1.0, 0)),
            shadow_projection * glm::lookAt(transform->position, transform->position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0, -1.0, 0)),
            shadow_projection * glm::lookAt(transform->position, transform->position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0, 0.0, 1)),
            shadow_projection * glm::lookAt(transform->position, transform->position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0, 0, -1.0)),
            shadow_projection * glm::lookAt(transform->position, transform->position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0, -1.0, 0)),
            shadow_projection * glm::lookAt(transform->position, transform->position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0, -1.0, 0)),
        };

        // Could be done with a loop but i dont fucking care
        shader.SetUniformMat4("shadow_matrices[0]", shadow_transforms[0]);
        shader.SetUniformMat4("shadow_matrices[1]", shadow_transforms[1]);
        shader.SetUniformMat4("shadow_matrices[2]", shadow_transforms[2]);
        shader.SetUniformMat4("shadow_matrices[3]", shadow_transforms[3]);
        shader.SetUniformMat4("shadow_matrices[4]", shadow_transforms[4]);
        shader.SetUniformMat4("shadow_matrices[5]", shadow_transforms[5]);
        shader.SetUniform3f("light_position", transform->position.x, transform->position.y, transform->position.z);
        shader.SetUniform1f("far", shadow_far_plane);

        for (auto e : v) {
            auto &meshRenderer = Scene::Main->EntityRegistry.get<MeshRenderer>(e);
            auto &transform = Scene::Main->EntityRegistry.get<Transform>(e);
            if (meshRenderer.mesh) {
                transform.Update();
                shader.SetUniformMat4("transformModel", transform.model);
                meshRenderer.mesh->Draw(shader);
            }
        }

        for (auto e : v2) {
            auto &meshRenderer = Scene::Main->EntityRegistry.get<Text3D>(e);
            auto &transform = Scene::Main->EntityRegistry.get<Transform>(e);
            transform.Update();
            shader.SetUniform1i("isText", 1);
            shader.SetUniformMat4("transformModel", transform.model);
            meshRenderer.Draw(shader);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        VaultRenderer::Window::window->AspectRatioCameraViewport();
    }

    void PointLight::OnGUI() {
        DrawComponent<PointLight>(Scene::Main->EntityRegistry, 40, [&] {
            ImGui::DragFloat("Intensity", &intensity, 0.01f, 0.0);
            ImGui::DragFloat("Shadow Far Plane", &shadow_far_plane, 0.1f, 0.0);
            ImGui::Checkbox("Shadows", &enable_shadow_mapping);
            ImGui::ColorEdit3("Color", &color.x);
        });
    }
} // namespace Engine::Components