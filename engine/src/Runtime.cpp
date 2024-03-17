#include "Engine/Components/Camera.hpp"
#include "Engine/Components/SpriteRenderer.hpp"
#include "Engine/Components/SpritesheetRenderer.hpp"
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
        instance = this;
    }

    void Runtime::SetDefaultShader(VaultRenderer::Shader *default_shader) {
        Runtime::default_shader = default_shader;
    }

    static void UpdateCXXScripts() {
        auto v = Scene::Main->EntityRegistry.view<CXXScriptComponent>();

        for (auto e : v) {
            auto &component = Scene::Main->EntityRegistry.get<CXXScriptComponent>(e);
            component.Update();
        }
    }

    void Runtime::UpdateGameObjects(Window &window) {
        static double lastTime = 0;
        double now = glfwGetTime();
        timestep = now - lastTime;
        lastTime = now;

        // glfwGetWindowSize(VaultRenderer::Window::window->GetGLFWWindow(), &width, &height);
        // glViewport(0, 0, width, height);
        // UpdateMainCamera(window);

        if (paused)
            return;
        if (Scene::Main == nullptr)
            return;

        default_shader->Bind();
        SetGlobalUniforms();
        Scene::Main->OnRuntimeUpdate(timestep);
        Scene::Main->UpdateGameObjectComponents();
        Scene::UpdateStaticGameObjectComponents();

        // Depth camera rendering !!
        std::vector<Camera *> depth_cameras = {};
        auto v = Scene::Main->EntityRegistry.view<Camera>();

        // for (auto &e : v) {
        //     auto &camera = Scene::Main->EntityRegistry.get<Camera>(e);

        //     if (camera.depth_camera) depth_cameras.push_back(&camera);
        // }

        // if (depth_cameras.size() > 0 && Scene::Main->main_camera_object != Scene::Main->EditorSceneCamera) {
        //     glClear(GL_DEPTH_BUFFER_BIT);

        //     for (auto *camera : depth_cameras) {
        //         camera->UpdateMatrix();
        //         camera->BindToShader(*default_shader);
        //         camera->BindToShader(*Font::font_shader);
        //         camera->width = VaultRenderer::Window::window->width;
        //         camera->height = VaultRenderer::Window::window->height;

        //         std::cout << " maan wtf \n";
        //         for (auto gameObject : Scene::Main->GameObjects) {
        //             gameObject->UpdateRendering(*Runtime::default_shader, true);
        //         }
        //     }
        // }

        glDisable(GL_BLEND);

        std::vector<int> to_remove;
        for (int i = 0; i < main_thread_calls.size(); i++) {
            main_thread_calls[i]();
            to_remove.push_back(i);
        }

        for (int i : to_remove) {
            main_thread_calls.erase(main_thread_calls.begin() + i);
        }
        to_remove.clear();
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

    void Runtime::AspectRatioCameraViewport() {
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
        auto v_model = Scene::Main->EntityRegistry.view<ModelRenderer>();
        auto v_text = Scene::Main->EntityRegistry.view<Text3D>();
        auto v_sprite = Scene::Main->EntityRegistry.view<SpriteRenderer>();
        auto v_spritesheet = Scene::Main->EntityRegistry.view<SpritesheetRenderer>();

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

        for (auto e : v_model) {
            auto &modelRenderer = Scene::Main->EntityRegistry.get<ModelRenderer>(e);
            auto &transform = Scene::Main->EntityRegistry.get<Transform>(e);
            if (modelRenderer.model) {
                transform.Update();
                shader.SetUniformMat4("transformModel", transform.model);
                modelRenderer.AnimateAndSetUniforms(shader);
                modelRenderer.Draw(shader);
            }
        }

        for (auto e : v_text) {
            auto &text3D = Scene::Main->EntityRegistry.get<Text3D>(e);
            auto &transform = Scene::Main->EntityRegistry.get<Transform>(e);
            shader.SetUniform1i("isText", 1);
            text3D.Draw(shader);
        }

        for (auto e : v_sprite) {
            auto &sprite = Scene::Main->EntityRegistry.get<SpriteRenderer>(e);
            auto &transform = Scene::Main->EntityRegistry.get<Transform>(e);
            transform.Update();
            shader.SetUniformMat4("transformModel", transform.model);
            sprite.Draw(shader);
        }

        for (auto e : v_spritesheet) {
            auto &sprite = Scene::Main->EntityRegistry.get<SpritesheetRenderer>(e);
            auto &transform = Scene::Main->EntityRegistry.get<Transform>(e);
            transform.Update();
            shader.SetUniformMat4("transformModel", transform.model);
            sprite.Draw(shader);
        }

        shadowMap.Unbind();
        VaultRenderer::Window::window->AspectRatioCameraViewport();
    }

    void Runtime::ShadowShenanigans(VaultRenderer::ShadowMap &shadow_map) {
        BindShadowsToShader(*default_shader, shadow_map);
        if (Scene::Main->EntityRegistry.valid(Scene::Main->EntityRegistry.view<DirectionalLight>().back())) { // Check if the last element of DirLights is a valid entity and not a deleteed one
            auto &light = Scene::Main->EntityRegistry.get<DirectionalLight>(Scene::Main->EntityRegistry.view<DirectionalLight>().back());
            if (light.enable_shadow_mapping) {
                shadow_map.CalculateMatrices(light.transform->position, Scene::Main->main_camera_object->transform->position, Scene::Main->main_camera_object->front);
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
            shader.SetUniform1i("shadowCubemap", 11);
            shader.SetUniform1i("shadow_cubemap_mapping", 0);
        }
    }
} // namespace Engine