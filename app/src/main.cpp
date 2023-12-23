#include "Engine/GameObject.hpp"
#include <iostream>
#include <Renderer/Window.hpp>
#include <Renderer/Mesh.hpp>
#include <Renderer/Texture.hpp>
#include <imgui/imgui.h>
#include <Engine/Scene.hpp>
#include <Engine/Components/IncludeComponents.hpp>

static VaultRenderer::Shader *default_shader;

void UpdateGameObjects() {
    using namespace Engine;
    using namespace Engine::Components;
    for (auto gameObject : Scene::Main->GameObjects) {
        if (gameObject->HasComponent<MeshRenderer>()) {

            auto &meshRenderer = gameObject->GetComponent<MeshRenderer>();
            if (meshRenderer.mesh) {
                meshRenderer.mesh->Draw(*default_shader);
            }
        }

        if (gameObject->HasComponent<AmbientLight>()) {
            auto &light = gameObject->GetComponent<AmbientLight>();
            light.AttachToShader(*default_shader);
        }

        if (gameObject->HasComponent<PointLight>()) {
            auto &light = gameObject->GetComponent<PointLight>();
            light.AttachToShader(*default_shader);
        }
    }
}

int main() {
    using namespace VaultRenderer;
    Window window(800, 600, "Vault Engine");

    Shader shader("../shaders/default.glsl");
    default_shader = &shader;

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
    meshRenderer.mesh->material.SetDiffuse("../assets/planks.png");
    meshRenderer.mesh->material.SetSpecular("../assets/planksSpec.png");

    auto cameraObject = GameObject::New("Camera");
    cameraObject->AddComponent<Components::Camera>();

    auto lightObject = GameObject::New("PointLight");
    lightObject->AddComponent<Components::PointLight>();

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

    window.Run([&] {
        camera.width = window.width;
        camera.height = window.height;
        window.SetClearColor(0x000000);
        transform.Update();
        shader.SetUniformMat4("transformModel", transform.model);

        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);

        // model = glm::rotate(m/* */odel, glm::radians(rotation), glm::vec3(0.f, 1.f, 0.f));
        // view = glm::translate(view, glm::vec3(0.0f, -0.5f, -2.0f));
        // projection = glm::perspective(glm::radians(45.0f), (float)(800.f / 600.f), 0.01f, 100.0f);
        // shader.SetUniformMat4("camera_view", view);
        // shader.SetUniformMat4("camera_projection", projection);

        // square.Draw(shader);
        // auto meshRendererView = Scene::Main->EntityRegistry.view<Components::MeshRenderer>();
        camera.UpdateMatrix();
        camera.BindToShader(shader);
        camera.Inputs();

        UpdateGameObjects();

        if (ImGui::Begin("Hierarchy")) {
            for (auto &gameObject : Scene::Main->GameObjects) {
                if (gameObject) {
                    ImGui::Text("%s", gameObject->name.c_str());
                    ImGui::NewLine();
                }
            }
            ImGui::DragFloat3("Pos", &light_transform.position.x, 0.01f);
            ImGui::End();
        }
    });

    return 0;
}