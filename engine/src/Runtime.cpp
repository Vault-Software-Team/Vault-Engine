#include <Engine/Runtime.hpp>
#include <Engine/Components/IncludeComponents.hpp>
#include <iostream>

using namespace Engine::Components;
using namespace VaultRenderer;
namespace Engine {
    DLL_API Runtime *Runtime::instance = nullptr;
    DLL_API VaultRenderer::Shader *Runtime::default_shader = nullptr;

    Runtime::Runtime(VaultRenderer::Shader *default_shader) {
        Runtime::default_shader = default_shader;
    }

    void Runtime::SetDefaultShader(VaultRenderer::Shader *default_shader) {
        Runtime::default_shader = default_shader;
    }

    void Runtime::UpdateGameObjects(Window &window) {
        // glfwGetWindowSize(VaultRenderer::Window::window->GetGLFWWindow(), &width, &height);
        // glViewport(0, 0, width, height);
        // UpdateMainCamera(window);

        if (paused)
            return;
        if (Scene::Main == nullptr)
            return;
        default_shader->Bind();
        SetGlobalUniforms();
        Scene::Main->UpdateGameObjectComponents();
        Scene::UpdateStaticGameObjectComponents();
        glDisable(GL_BLEND);
    }

    void Runtime::Pause() {
        paused = true;
    }

    void Runtime::Unpause() {
        paused = false;
    }

    bool Runtime::Paused() const {
        return paused;
    }

    void Runtime::SetGlobalUniforms() {
        default_shader->SetUniform1i("point_light_count", Engine::Scene::Main->EntityRegistry.view<Engine::Components::PointLight>().size());
        default_shader->SetUniform1i("dir_light_count", Engine::Scene::Main->EntityRegistry.view<Engine::Components::DirectionalLight>().size());
        default_shader->SetUniform1i("spot_light_count", Engine::Scene::Main->EntityRegistry.view<Engine::Components::SpotLight>().size());
    }

    void AspectRatioCameraViewport() {
        const int targetWidth = 1920, targetHeight = 1080;
        float targetAspectRatio = (float)targetWidth / (float)targetHeight;

        int aspectWidth = VaultRenderer::Window::window->width;
        int aspectHeight = (int)((float)aspectWidth / targetAspectRatio);
        if (aspectHeight > VaultRenderer::Window::window->height) {
            aspectHeight = VaultRenderer::Window::window->height;
            aspectWidth = (int)((float)aspectHeight * targetAspectRatio);
        }
        int vpX = (int)(((float)VaultRenderer::Window::window->width / 2.0f) - ((float)aspectWidth / 2.0f));
        int vpY = (int)(((float)VaultRenderer::Window::window->height / 2.0f) - ((float)aspectHeight / 2.0f));

        glViewport(vpX, vpY, aspectWidth, aspectHeight);
    }

    void Runtime::UpdateMainCamera(Window &window) {
        if (!Scene::Main->main_camera_object)
            return;

        Scene::Main->main_camera_object->UpdateMatrix();
        Scene::Main->main_camera_object->BindToShader(*default_shader);
        Scene::Main->main_camera_object->Inputs();

        if (Engine::Scene::Main->main_camera_object) {
            Engine::Scene::Main->main_camera_object->width = VaultRenderer::Window::window->width;
            Engine::Scene::Main->main_camera_object->height = VaultRenderer::Window::window->height;
        }
        // AspectRatioCameraViewport();
    }

    // ---- SHADOW FUCKERY ---- //
    void Runtime::DrawToShadowMap(VaultRenderer::DepthFramebuffer &shadowMap, VaultRenderer::Shader &shader) {
        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, shadowMap.width, shadowMap.height);
        shadowMap.Bind();
        glClear(GL_DEPTH_BUFFER_BIT);
        auto v = Scene::Main->EntityRegistry.view<MeshRenderer>();
        auto v_text = Scene::Main->EntityRegistry.view<Text3D>();

        shader.Bind();

        for (auto e : v) {
            auto &meshRenderer = Scene::Main->EntityRegistry.get<MeshRenderer>(e);
            auto &transform = Scene::Main->EntityRegistry.get<Transform>(e);
            if (meshRenderer.mesh) {
                transform.Update();
                shader.SetUniformMat4("transformModel", transform.model);
                meshRenderer.mesh->Draw(shader);
            }
        }

        for (auto e : v_text) {
            auto &text3D = Scene::Main->EntityRegistry.get<Text3D>(e);
            auto &transform = Scene::Main->EntityRegistry.get<Transform>(e);
            text3D.Draw(shader);
        }

        shadowMap.Unbind();
        glViewport(0, 0, VaultRenderer::Window::window->width, VaultRenderer::Window::window->height);
    }

    void Runtime::ShadowShenanigans(VaultRenderer::ShadowMap &shadow_map) {
        BindShadowsToShader(*default_shader, shadow_map);
        if (Scene::Main->EntityRegistry.valid(Scene::Main->EntityRegistry.view<DirectionalLight>().back())) { // Check if the last element of DirLights is a valid entity and not a deleteed one
            auto &light = Scene::Main->EntityRegistry.get<DirectionalLight>(Scene::Main->EntityRegistry.view<DirectionalLight>().back());
            if (light.enable_shadow_mapping) {
                shadow_map.CalculateMatrices(light.transform->position);
                shadow_map.SetLightProjection(*default_shader);
            }
        }
    }

    void Runtime::BindShadowsToShader(VaultRenderer::Shader &shader, VaultRenderer::ShadowMap &shadow_map) {
        // Bind the shader
        shader.Bind();

        // Set DirectionalLight shadow properties
        if (Scene::Main->EntityRegistry.valid(Scene::Main->EntityRegistry.view<DirectionalLight>().back())) {
            if (Scene::Main->EntityRegistry.get<DirectionalLight>(Scene::Main->EntityRegistry.view<DirectionalLight>().back()).enable_shadow_mapping) {
                shadow_map.BindTexture(10);
                shader.SetUniform1i("shadowMap", 10);
                shader.SetUniform1i("shadow_mapping", 1);
            } else {
                shader.SetUniform1i("shadow_mapping", 0);
            }
        } else {
            shader.SetUniform1i("shadow_mapping", 0);
        }
        // Set Point Light shadow properties

        PointLight *pLight = nullptr;
        auto v_PointLight = Scene::Main->EntityRegistry.view<PointLight>();

        PointLight *mainPointLight = nullptr;
        for (auto e : v_PointLight) {
            auto &lPLight = Scene::Main->EntityRegistry.get<PointLight>(e);
            if (!lPLight.enable_shadow_mapping)
                continue;

            if (mainPointLight) {
                if (Scene::Main) {
                    if (Scene::Main->main_camera_object) {
                        if (glm::distance(Scene::Main->main_camera_object->transform->position, lPLight.transform->position) < glm::distance(Scene::Main->main_camera_object->transform->position, mainPointLight->transform->position)) {
                            mainPointLight = &lPLight;
                        }
                    }
                }
            } else {
                mainPointLight = &lPLight;
            }
        }

        if (mainPointLight && mainPointLight->enable_shadow_mapping) {
            glActiveTexture(GL_TEXTURE11);
            glBindTexture(GL_TEXTURE_CUBE_MAP, mainPointLight->cubemap);
            shader.SetUniform1i("shadowCubemap", 11);
            shader.SetUniform1f("shadowCubemapFarPlane", mainPointLight->shadow_far_plane);
            shader.SetUniform1i("shadow_cubemap_mapping", 1);
        } else {
            shader.SetUniform1i("shadow_cubemap_mapping", 0);
        }
    }
} // namespace Engine