#include "Engine/Components/ModelRenderer.hpp"
#include "Engine/Components/SpriteRenderer.hpp"
#include "Engine/Components/SpritesheetAnimator.hpp"
#include "Engine/Components/SpritesheetRenderer.hpp"
#include "imgui/imgui.h"
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
        auto v = Scene::Main->EntityRegistry.view<MeshRenderer>();
        auto v2 = Scene::Main->EntityRegistry.view<Text3D>();
        auto v3 = Scene::Main->EntityRegistry.view<ModelRenderer>();
        auto v4 = Scene::Main->EntityRegistry.view<SpriteRenderer>();
        auto v5 = Scene::Main->EntityRegistry.view<SpritesheetRenderer>();
        auto v6 = Scene::Main->EntityRegistry.view<SpritesheetAnimator>();

        glm::mat4 shadow_projection = glm::perspective(glm::radians(90.0f), 2048.f / 2048.f, 1.0f, shadow_far_plane);
        glm::mat4 shadow_transforms[] = {
            shadow_projection * glm::lookAt(transform->position, transform->position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0, -1.0, 0)),
            shadow_projection * glm::lookAt(transform->position, transform->position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0, -1.0, 0)),
            shadow_projection * glm::lookAt(transform->position, transform->position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0, 0.0, 1.0)),
            shadow_projection * glm::lookAt(transform->position, transform->position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0, 0, -1.0)),
            shadow_projection * glm::lookAt(transform->position, transform->position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0, -1.0, 0)),
            shadow_projection * glm::lookAt(transform->position, transform->position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0, -1.0, 0)),
        };

        // Could be done with a loop but i dont fucking care
        shader.Bind();
        shader.SetUniformMat4("shadow_matrices[0]", shadow_transforms[0]);
        shader.SetUniformMat4("shadow_matrices[1]", shadow_transforms[1]);
        shader.SetUniformMat4("shadow_matrices[2]", shadow_transforms[2]);
        shader.SetUniformMat4("shadow_matrices[3]", shadow_transforms[3]);
        shader.SetUniformMat4("shadow_matrices[4]", shadow_transforms[4]);
        shader.SetUniformMat4("shadow_matrices[5]", shadow_transforms[5]);
        shader.SetUniform3f("light_position", transform->position.x, transform->position.y, transform->position.z);
        shader.SetUniform1f("far", shadow_far_plane);

        glViewport(0, 0, 2048, 2048);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);

        for (auto e : v) {
            auto &meshRenderer = Scene::Main->EntityRegistry.get<MeshRenderer>(e);
            auto &transform = Scene::Main->EntityRegistry.get<Transform>(e);
            if (meshRenderer.mesh) {
                // transform.Update();
                shader.SetUniformMat4("transformModel", transform.model);
                meshRenderer.mesh->Draw(shader);
            }
        }

        for (auto e : v2) {
            auto &meshRenderer = Scene::Main->EntityRegistry.get<Text3D>(e);
            auto &transform = Scene::Main->EntityRegistry.get<Transform>(e);
            // transform.Update();
            shader.SetUniform1i("isText", 1);
            shader.SetUniformMat4("transformModel", transform.model);
            meshRenderer.Draw(shader);
        }

        for (auto e : v3) {
            auto &modelRenderer = Scene::Main->EntityRegistry.get<ModelRenderer>(e);
            auto &transform = Scene::Main->EntityRegistry.get<Transform>(e);
            // transform.Update();
            shader.SetUniformMat4("transformModel", transform.model);
            modelRenderer.AnimateAndSetUniforms(shader);
            modelRenderer.Draw(shader, transform.model);
        }

        for (auto e : v4) {
            auto &spriteRenderer = Scene::Main->EntityRegistry.get<SpriteRenderer>(e);
            auto &transform = Scene::Main->EntityRegistry.get<Transform>(e);
            glDisable(GL_CULL_FACE);
            // transform.Update();
            shader.Bind();
            shader.SetUniformMat4("transformModel", transform.model);
            shader.SetUniform1ui("u_EntityID", (uint32_t)entity);
            shader.SetUniform1i("mesh_isFlat", true);
            shader.SetUniform1i("u_UseTextureAlpha", 1);
            spriteRenderer.Draw(shader);
            shader.SetUniform1i("u_UseTextureAlpha", 0);
        }

        for (auto e : v5) {
            auto &spritesheetRenderer = Scene::Main->EntityRegistry.get<SpritesheetRenderer>(e);
            auto &transform = Scene::Main->EntityRegistry.get<Transform>(e);
            glDisable(GL_CULL_FACE);
            // transform.Update();
            shader.Bind();
            shader.SetUniformMat4("transformModel", transform.model);
            // Runtime::instance->c_ShadowMap->BindToShader(shader);
            shader.SetUniform1ui("u_EntityID", (uint32_t)entity);
            shader.SetUniform1i("u_UseTextureAlpha", 1);
            spritesheetRenderer.Draw(shader);
            shader.SetUniform1i("u_UseTextureAlpha", 0);
        }

        for (auto e : v6) {
            auto &spritesheetRenderer = Scene::Main->EntityRegistry.get<SpritesheetAnimator>(e);
            auto &transform = Scene::Main->EntityRegistry.get<Transform>(e);
            glDisable(GL_CULL_FACE);
            // transform.Update();
            shader.Bind();
            shader.SetUniformMat4("transformModel", transform.model);
            // Runtime::instance->c_ShadowMap->BindToShader(shader);
            shader.SetUniform1ui("u_EntityID", (uint32_t)entity);
            shader.SetUniform1i("u_UseTextureAlpha", 1);
            spritesheetRenderer.Draw(shader);
            shader.SetUniform1i("u_UseTextureAlpha", 0);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        VaultRenderer::Window::window->AspectRatioCameraViewport();
        // VaultRenderer::Window::window->SetViewport(VaultRenderer::Window::window->width, VaultRenderer::Window::window->height);
    }

    void PointLight::OnGUI() {
        DrawComponent<PointLight>(Scene::Main->EntityRegistry, 40, [&] {
            ImGui::DragFloat("Intensity", &intensity, 0.01f, 0.0);
            ImGui::ColorEdit3("Color", &color.x, ImGuiColorEditFlags_HDR);
            ImGui::DragFloat("Shadow Far Plane", &shadow_far_plane, 0.1f, 0.0);
            ImGui::Checkbox("Shadows", &enable_shadow_mapping);
            // ImGui::DragFloat("Shadow Strength", &ShadowStrength, 0.1f, 0.0);
            // if (ShadowStrength < 0) ShadowStrength = 0;
        });
    }
} // namespace Engine::Components