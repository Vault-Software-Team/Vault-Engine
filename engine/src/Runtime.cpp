#include "Editor/EditorLayer.hpp"
#include "Editor/GUI/MainGUI.hpp"
#include "Engine/Components/BoxCollider2D.hpp"
#include "Engine/Components/CSharpScriptComponent.hpp"
#include "Engine/Components/Camera.hpp"
#include "Engine/Components/Collider3D.hpp"
#include "Engine/Components/MeshRenderer.hpp"
#include "Engine/Components/SpriteRenderer.hpp"
#include "Engine/Components/SpritesheetRenderer.hpp"
#include "Engine/Components/Transform.hpp"
#include "Engine/Physics/BulletPhysics.hpp"
#include "Engine/SimpleCalls.hpp"
#include <Engine/Runtime.hpp>
#include <Engine/Components/IncludeComponents.hpp>
#include <iostream>
#include <memory>

using namespace Engine::Components;
using namespace VaultRenderer;
namespace Engine {
    DLL_API Runtime *Runtime::instance = nullptr;
    DLL_API VaultRenderer::Shader *Runtime::default_shader = nullptr;
    bool Runtime::usePBR = false;

    Runtime::Runtime(VaultRenderer::Shader *default_shader) {
        Runtime::default_shader = default_shader;
        instance = this;
        bulletPhysics = std::make_unique<Physics3D>();
    }

    Runtime::~Runtime() {
        // delete animThread;
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
        if (isRunning)
            Scene::Main->OnRuntimeUpdate(timestep);
        Scene::Main->UpdateGameObjectComponents();
        Scene::UpdateStaticGameObjectComponents();
        bulletPhysics->UpdatePhysics();
        std::vector<Camera *> depth_cameras = {};
        auto camV = Scene::Main->EntityRegistry.view<Camera>();

#ifndef GAME_BUILD

        if (Editor::GUI::selected_gameObject && Editor::EditorLayer::instance->EnableColliderGizmo) {
            auto &gameObject = Editor::GUI::selected_gameObject;

            if (gameObject->HasComponent<MeshCollider3D>() && gameObject->HasComponent<MeshRenderer>()) {
                glDisable(GL_DEPTH_TEST);
                glDepthFunc(GL_LEQUAL);
                glDisable(GL_CULL_FACE);

                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

                auto &comp = gameObject->GetComponent<MeshRenderer>();
                auto &coll = gameObject->GetComponent<MeshCollider3D>();
                auto &transform = gameObject->GetComponent<Transform>();

                Editor::EditorLayer::instance->ColliderGizmo.shader->Bind();
                Editor::EditorLayer::instance->ColliderGizmo.shader->SetUniform1i("texture_diffuse.defined", 0);
                Editor::EditorLayer::instance->ColliderGizmo.DrawGizmo(*Editor::EditorLayer::instance->ColliderGizmo.shader, comp.mesh.get(), transform.position, transform.rotation, transform.scale, coll.size);

                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                glEnable(GL_DEPTH_TEST);
            }

            if (gameObject->HasComponent<BoxCollider3D>()) {
                glDisable(GL_DEPTH_TEST);
                glDepthFunc(GL_LEQUAL);
                glDisable(GL_CULL_FACE);
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                auto &coll = gameObject->GetComponent<BoxCollider3D>();
                auto &transform = gameObject->GetComponent<Transform>();

                Editor::EditorLayer::instance->ColliderGizmo.shader->Bind();
                Editor::EditorLayer::instance->ColliderGizmo.shader->SetUniform1i("texture_diffuse.defined", 0);
                Editor::EditorLayer::instance->ColliderGizmo.DrawGizmo(*Editor::EditorLayer::instance->ColliderGizmo.shader, &MeshRenderer::ModelMeshes[MESH_CUBE]->meshes.back(), transform.position, transform.rotation, transform.scale, coll.size);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                glEnable(GL_DEPTH_TEST);
            }

            if (gameObject->HasComponent<BoxCollider2D>()) {
                glDisable(GL_DEPTH_TEST);
                glDepthFunc(GL_LEQUAL);
                glDisable(GL_CULL_FACE);
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                auto &coll = gameObject->GetComponent<BoxCollider2D>();
                auto &transform = gameObject->GetComponent<Transform>();

                Editor::EditorLayer::instance->ColliderGizmo.shader->Bind();
                glm::mat4 model = glm::translate(glm::mat4(1.0f), transform.position) *
                                  glm::toMat4(glm::quat(transform.rotation)) *
                                  glm::scale(glm::mat4(1.0f), glm::vec3((((coll.size.x) / 2) - 0.02) / 2, (((coll.size.y) / 2) - 0.02) / 2, 1));

                Editor::EditorLayer::instance->ColliderGizmo.shader->SetUniformMat4("transformModel", model);
                SimpleCalls::RenderQuad();
                // mesh->Draw(shader, false);

                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                glEnable(GL_DEPTH_TEST);
            }
        }

#endif

// Draw Component Icons
#ifndef GAME_BUILD
        glDisable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glDisable(GL_CULL_FACE);
        if (Scene::Main->main_camera_object == Scene::Main->EditorSceneCamera && Scene::Main->main_camera_object && Editor::EditorLayer::instance->EnableIconGizmo) {
            Scene::Main->main_camera_object->BindToShader(*Editor::EditorLayer::instance->ColliderGizmo.shader);

            auto &camera = Scene::Main->StaticGameObjects_EntityRegistry.get<Camera>(Scene::Main->main_camera_object->entity);
            for (auto &e : camV) {
                auto &transform = Scene::Main->EntityRegistry.get<Transform>(e);

                Transform t = transform;
                float distance = glm::distance(t.position, camera.transform->position);
                t.scale = glm::vec3(-(distance / 8), distance / 8, distance / 8);
                t.LookAt(camera.transform->position);
                t.Update();

                Editor::EditorLayer::instance->ColliderGizmo.shader->Bind();
                Editor::EditorLayer::instance->ColliderGizmo.shader->SetUniformMat4("transformModel", t.model);
                Editor::EditorLayer::instance->ColliderGizmo.shader->SetUniform1ui("u_EntityID", (uint32_t)transform.entity);
                Editor::EditorLayer::instance->icons.CameraIcon->Bind(0);
                Editor::EditorLayer::instance->ColliderGizmo.shader->SetUniform1i("texture_diffuse.defined", 1);
                Editor::EditorLayer::instance->ColliderGizmo.shader->SetUniform1i("texture_diffuse.tex", 0);
                SimpleCalls::RenderQuad();
                // Editor::EditorLayer::instance->DrawIcon(Editor::EditorLayer::instance->iconMeshes.CameraIcon, *Editor::EditorLayer::instance->ColliderGizmo.shader, t.model, (uint32_t)transform.entity, Editor::EditorLayer::instance->icons.CameraIcon);
            }

            auto v_PointLight = Scene::Main->EntityRegistry.view<PointLight>();
            for (auto &e : v_PointLight) {
                auto &transform = Scene::Main->EntityRegistry.get<Transform>(e);
                Transform t = transform;
                float distance = glm::distance(t.position, camera.transform->position);
                t.scale = glm::vec3(-(distance / 8), distance / 8, distance / 8);
                t.LookAt(camera.transform->position);
                t.Update();

                Editor::EditorLayer::instance->ColliderGizmo.shader->Bind();
                Editor::EditorLayer::instance->ColliderGizmo.shader->SetUniformMat4("transformModel", t.model);
                Editor::EditorLayer::instance->ColliderGizmo.shader->SetUniform1ui("u_EntityID", (uint32_t)transform.entity);
                Editor::EditorLayer::instance->icons.PointLightIcon->Bind(0);
                Editor::EditorLayer::instance->ColliderGizmo.shader->SetUniform1i("texture_diffuse.defined", 1);
                Editor::EditorLayer::instance->ColliderGizmo.shader->SetUniform1i("texture_diffuse.tex", 0);
                SimpleCalls::RenderQuad();
            }

            auto v_DirectionalLight = Scene::Main->EntityRegistry.view<DirectionalLight>();
            for (auto &e : v_DirectionalLight) {
                auto &transform = Scene::Main->EntityRegistry.get<Transform>(e);
                Transform t = transform;
                float distance = glm::distance(t.position, camera.transform->position);
                t.scale = glm::vec3(-(distance / 8), distance / 8, distance / 8);
                t.LookAt(camera.transform->position);
                t.Update();

                Editor::EditorLayer::instance->ColliderGizmo.shader->Bind();
                Editor::EditorLayer::instance->ColliderGizmo.shader->SetUniformMat4("transformModel", t.model);
                Editor::EditorLayer::instance->ColliderGizmo.shader->SetUniform1ui("u_EntityID", (uint32_t)transform.entity);
                Editor::EditorLayer::instance->icons.DirLightIcon->Bind(0);
                Editor::EditorLayer::instance->ColliderGizmo.shader->SetUniform1i("texture_diffuse.defined", 1);
                Editor::EditorLayer::instance->ColliderGizmo.shader->SetUniform1i("texture_diffuse.tex", 0);
                SimpleCalls::RenderQuad();
            }

            auto v_SpotLight = Scene::Main->EntityRegistry.view<SpotLight>();
            for (auto &e : v_SpotLight) {
                auto &transform = Scene::Main->EntityRegistry.get<Transform>(e);
                Transform t = transform;
                float distance = glm::distance(t.position, camera.transform->position);
                t.scale = glm::vec3(-(distance / 8), distance / 8, distance / 8);
                t.LookAt(camera.transform->position);
                t.Update();

                Editor::EditorLayer::instance->ColliderGizmo.shader->Bind();
                Editor::EditorLayer::instance->ColliderGizmo.shader->SetUniformMat4("transformModel", t.model);
                Editor::EditorLayer::instance->ColliderGizmo.shader->SetUniform1ui("u_EntityID", (uint32_t)transform.entity);
                Editor::EditorLayer::instance->icons.SpotLightIcon->Bind(0);
                Editor::EditorLayer::instance->ColliderGizmo.shader->SetUniform1i("texture_diffuse.defined", 1);
                Editor::EditorLayer::instance->ColliderGizmo.shader->SetUniform1i("texture_diffuse.tex", 0);
                SimpleCalls::RenderQuad();
            }
        }
        glEnable(GL_DEPTH_TEST);

#endif

        // Depth camera rendering !!

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
        // window.SetViewport(width, height);
        AspectRatioCameraViewport();
    }

    // ---- SHADOW FUCKERY ---- //
    void Runtime::DrawToShadowMap(VaultRenderer::DepthFramebuffer &shadowMap, VaultRenderer::Shader &shader) {
        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, shadowMap.width, shadowMap.height);
        shadowMap.Bind();
        glClear(GL_DEPTH_BUFFER_BIT);
        // c_ShadowMap->BindForDrawing();
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
                // shader.SetUniformMat4("transformModel", transform.model);
                modelRenderer.AnimateAndSetUniforms(shader);
                modelRenderer.Draw(shader, transform.model);
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
        // c_ShadowMap->UnbindDrawing();
        // Window::window->SetViewport(width, height);
        VaultRenderer::Window::window->AspectRatioCameraViewport();
    }

    void Runtime::ShadowShenanigans(VaultRenderer::ShadowMap &shadow_map) {
        BindShadowsToShader(*default_shader, shadow_map);
        if (Scene::Main->EntityRegistry.valid(Scene::Main->EntityRegistry.view<DirectionalLight>().back())) { // Check if the last element of DirLights is a valid entity and not a deleteed one
            auto &light = Scene::Main->EntityRegistry.get<DirectionalLight>(Scene::Main->EntityRegistry.view<DirectionalLight>().back());
            c_ShadowMap->lightDir = &light.transform->position;
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
            auto &comp = Scene::Main->EntityRegistry.get<DirectionalLight>(Scene::Main->EntityRegistry.view<DirectionalLight>().back());
            if (comp.enable_shadow_mapping) {
                // c_ShadowMap->BindMap(10);
                shadow_map.BindTexture(10);
                shader.SetUniform1i("m_shadowMap", 10);
                shader.SetUniform1i("cameraFarPlane", 500.f);
                shader.SetUniform1i("shadow_mapping", 1);
                shader.SetUniform3f("c_ShadowMapLightDir", comp.transform->position.x, comp.transform->position.y, comp.transform->position.z);
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

    void Runtime::MouseEvents(entt::entity hoveredEntity) {
        // This here are mouse events for the C# scripts

        static entt::entity currentHoveredEntity = (entt::entity)65555;
        if (!isRunning) return;

        if (currentHoveredEntity != hoveredEntity) {
            if (Scene::Main->EntityRegistry.all_of<CSharpScriptComponent>(currentHoveredEntity)) {
                auto &comp = Scene::Main->EntityRegistry.get<CSharpScriptComponent>(currentHoveredEntity);
                for (auto &instance : comp.script_instances) {
                    instance.second->OnMouseExit();
                }

                currentHoveredEntity = hoveredEntity;
            }
        }

        if (Scene::Main->EntityRegistry.all_of<CSharpScriptComponent>(hoveredEntity)) {
            auto &comp = Scene::Main->EntityRegistry.get<CSharpScriptComponent>(hoveredEntity);
            currentHoveredEntity = hoveredEntity;

            for (auto &instance : comp.script_instances) {
                instance.second->OnMouseEnter();
            }
        }
    }
} // namespace Engine