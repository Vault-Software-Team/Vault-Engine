#include "Engine/GameObject.hpp"
#include "Engine/Model.hpp"
#include <iostream>
#include <Renderer/Window.hpp>
#include <Renderer/Mesh.hpp>
#include <Renderer/Texture.hpp>
#include <Renderer/Skybox.hpp>
#include <imgui/imgui.h>
#include <Engine/Scene.hpp>
#include <Engine/Components/IncludeComponents.hpp>
#include <Renderer/ShadowMap.hpp>
#include <Renderer/Stats.hpp>

static VaultRenderer::Shader *default_shader;

void SetGlobalUniforms() {
    default_shader->SetUniform1i("point_light_count", Engine::Scene::Main->EntityRegistry.view<Engine::Components::PointLight>().size());
    default_shader->SetUniform1i("dir_light_count", Engine::Scene::Main->EntityRegistry.view<Engine::Components::DirectionalLight>().size());
    default_shader->SetUniform1i("spot_light_count", Engine::Scene::Main->EntityRegistry.view<Engine::Components::SpotLight>().size());
}

using namespace Engine;
using namespace VaultRenderer;
using namespace Engine::Components;

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

    if (Engine::Scene::Main->main_camera_object) {
        Engine::Scene::Main->main_camera_object->width = 1920;
        Engine::Scene::Main->main_camera_object->height = 1080;
    }

    glViewport(vpX, vpY, aspectWidth, aspectHeight);
}

void DrawToShadowMap(DepthFramebuffer &shadowMap, Shader &shader) {
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, shadowMap.width, shadowMap.height);
    shadowMap.Bind();
    glClear(GL_DEPTH_BUFFER_BIT);
    auto v = Scene::Main->EntityRegistry.view<MeshRenderer>();

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

    shadowMap.Unbind();
    glViewport(0, 0, Window::window->width, Window::window->height);
    // AspectRatioCameraViewport();
}

void UpdateGameObjects() {
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    default_shader->Bind();
    SetGlobalUniforms();
    for (auto gameObject : Scene::Main->GameObjects) {
        // if (gameObject->parent != "NO_PARENT")
        gameObject->UpdateComponents(*default_shader);
    }
    glDisable(GL_BLEND);
}

void OnGUI(Engine::Components::Transform &light_transform) {
    using namespace VaultRenderer;
    using namespace Engine;
    using namespace Engine::Components;
    if (ImGui::Begin("Hierarchy")) {
        for (auto &gameObject : Scene::Main->GameObjects) {
            if (gameObject) {
                ImGui::Text("%s", gameObject->name.c_str());
                ImGui::DragFloat3((gameObject->name + "h").c_str(), &gameObject->GetComponent<Transform>().position.x, 0.01f);
                ImGui::DragFloat3((gameObject->name + "a").c_str(), &gameObject->GetComponent<Transform>().rotation.x, 0.01f);
                ImGui::NewLine();
            }
        }
        ImGui::DragFloat3("Pos", &light_transform.position.x, 0.01f);
        ImGui::DragFloat3("Rot", &light_transform.rotation.x, 0.01f);
        ImGui::End();
    }

    ImGui::Begin("Statistics");
    ImGui::Text("Draw Calls: %d", Statistics::GetDrawCalls());
    ImGui::Text("Vendor: %s", Statistics::vendor.c_str());
    ImGui::Text("Renderer: %s", Statistics::renderer.c_str());
    ImGui::Text("Version: %s", Statistics::version.c_str());
    ImGui::Text("Shading Language: %s", Statistics::shading_language.c_str());
    ImGui::End();
}

void BindShadowsToShader(Shader &shader, ShadowMap &shadow_map) {
    // Bind the shader
    shader.Bind();

    // Set DirectionalLight shadow properties
    shadow_map.BindTexture(10);
    shader.SetUniform1i("shadowMap", 10);
    // Set Point Light shadow properties

    PointLight *pLight = nullptr;
    auto v_PointLight = Scene::Main->EntityRegistry.view<PointLight>();

    PointLight *mainPointLight = nullptr;
    for (auto e : v_PointLight) {
        auto &lPLight = Scene::Main->EntityRegistry.get<PointLight>(e);

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

    if (mainPointLight) {
        glActiveTexture(GL_TEXTURE11);
        glBindTexture(GL_TEXTURE_CUBE_MAP, mainPointLight->cubemap);
        shader.SetUniform1i("shadowCubemap", 11);
        shader.SetUniform1f("shadowCubemapFarPlane", mainPointLight->shadow_far_plane);
    }
}

int main() {
    using namespace VaultRenderer;
    Window window(1280, 720, "Vault Engine");
    Statistics::SetStats();

    Shader shader("../shaders/default.glsl");
    Shader skybox_shader("../shaders/skybox.glsl");
    Shader shadow_map_shader("../shaders/shadow_map.glsl");
    Shader shadow_cubemap_shader("../shaders/shadow_map_point.glsl");
    default_shader = &shader;

    Skybox skybox;

    shader.Bind();

    float rotation = 0.5f;
    double previousTime = glfwGetTime();

    using namespace Engine;
    Scene::New("../assets/main.vault");

    auto gameObject = GameObject::New("My GameObject");
    gameObject->AddComponent<Components::MeshRenderer>();
    gameObject->AddComponent<Components::AmbientLight>();
    auto &meshRenderer = gameObject->GetComponent<Components::MeshRenderer>();
    gameObject->GetComponent<Components::AmbientLight>().amount = 0.2f;
    meshRenderer.SetMeshType(Components::MESH_PLANE);
    meshRenderer.mesh->material.SetDiffuse("../assets/diffuse.png");
    meshRenderer.mesh->material.SetSpecular("../assets/diffuse.png");
    meshRenderer.mesh->material.SetNormal("../assets/normal.png");

    auto cameraObject = GameObject::New("Camera");
    cameraObject->AddComponent<Components::Camera>();

    auto lightObject = GameObject::New("PointLight");
    lightObject->AddComponent<Components::PointLight>();
    lightObject->AddComponent<Components::MeshRenderer>();
    lightObject->GetComponent<Components::MeshRenderer>().SetMeshType(Components::MESH_PYRAMID);

    using namespace Engine::Components;
    auto &transform = gameObject->GetComponent<Transform>();
    auto &light_transform = lightObject->GetComponent<Transform>();
    transform.rotation.x = glm::radians(90.f);
    transform.scale = glm::vec3(10, 10, 10);
    auto &camera_transform = cameraObject->GetComponent<Transform>();
    auto &camera = cameraObject->GetComponent<Camera>();
    camera_transform.rotation.z = -1.0f;

    camera.fov = 45;
    camera.near = 0.01f;
    camera.far = 100.0f;
    camera.width = 800;
    camera.height = 600;
    camera_transform.position.y = 0.5f;
    camera_transform.position.z = 25.f;
    Scene::Main->SetMainCameraObject(cameraObject);

    Model model("../assets/capsule.obj");
    ShadowMap shadow_map;

    window.Run([&] {
        camera.width = window.width;
        camera.height = window.height;
        Statistics::ResetDrawCalls();
        window.SetClearColor(0x000000);

        // model = glm::rotate(m/* */odel, glm::radians(rotation), glm::vec3(0.f, 1.f, 0.f));
        // view = glm::translate(view, glm::vec3(0.0f, -0.5f, -2.0f));
        // projection = glm::perspective(glm::radians(45.0f), (float)(800.f / 600.f), 0.01f, 100.0f);
        // shader.SetUniformMat4("camera_view", view);
        // shader.SetUniformMat4("camera_projection", projection);

        // square.Draw(shader);
        // auto meshRendererView = Scene::Main->EntityRegistry.view<Components::MeshRenderer>();
        if (Scene::Main->main_camera_object) {
            Scene::Main->main_camera_object->UpdateMatrix();
            Scene::Main->main_camera_object->BindToShader(shader);
            Scene::Main->main_camera_object->Inputs();
        }

        skybox.Render(skybox_shader, camera_transform.position, camera_transform.rotation, camera.up);

        // AspectRatioCameraViewport();
        glViewport(0, 0, window.width, window.height);
        // DrawToShadowMap(shadowMap, lightProj, shadow_map_shader);
        BindShadowsToShader(shader, shadow_map);

        shadow_map.CalculateMatrices(light_transform.position);
        shadow_map.SetLightProjection(shader);

        UpdateGameObjects(); },
               [&] {
                   OnGUI(light_transform);
               },
               [&] {
                   // Directional Light  Shadow Mapping
                   shadow_map.RenderSpace([&](std::unique_ptr<Shader> &shadow_shader) {
                       // NOTE: shadow_shader is already binded
                       DrawToShadowMap(shadow_map.GetDepthBuffer(), *shadow_shader);
                   });

                   // Point Light Shadow Mapping
                   auto pointLightView = Scene::Main->EntityRegistry.view<PointLight>();

                   for (auto e : pointLightView) {
                       auto &light = Scene::Main->EntityRegistry.get<PointLight>(e);
                       light.DrawToShadowMap(shadow_cubemap_shader);
                   }
               });

    return 0;
}