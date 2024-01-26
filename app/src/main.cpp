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
#include <Renderer/Font.hpp>
#include <Engine/Runtime.hpp>
#include <Editor/EditorLayer.hpp>
#include <fstream>
#include <Editor/GUI/MainGUI.hpp>
#include <Engine/Batch.hpp>
#include <Engine/SceneSerialization.hpp>
#include <Engine/Scripting/LoadScripts.hpp>
#include <dlfcn.h>
#include <script_test.hpp>
#include <HyperScript/HyperScript.hpp>
static VaultRenderer::Shader *default_shader;

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

void OnGUI(uint32_t smID) {
    using namespace VaultRenderer;
    using namespace Engine;
    using namespace Engine::Components;
    // ImGui::Begin("Statistics");
    // ImGui::Text("Draw Calls: %d", Statistics::GetDrawCalls());
    // ImGui::Text("Vendor: %s", Statistics::vendor.c_str());
    // ImGui::Text("Renderer: %s", Statistics::renderer.c_str());
    // ImGui::Text("Version: %s", Statistics::version.c_str());
    // ImGui::Text("Shading Language: %s", Statistics::shading_language.c_str());
    // ImGui::End();
}

using namespace Editor;
void print(const std::string &traki) {
    std::cout << traki << "\n";
}
int main() {
    using namespace VaultRenderer;

    // ANGEL SCRIPT TEST
    // Scripting::AngelScript::AngelScriptEngine AsEngine;
    // AsEngine.RegisterGlobalFunction("void print(const string &in)", asFUNCTION(print), asCALL_CDECL);
    // AsEngine.LoadScript("test", "../assets/scripts/test.as");
    // AsEngine.CreateContext();
    // AsEngine.GetTypeInfoBydeclaration(AsEngine.GetModule("test"), "MyClass");
    // AsEngine.PrepareAndExecuteFunction(AsEngine.CreateContext(), AsEngine.GetFunctionByDeclaration(AsEngine.GetModule("test"), "void main()"));
    // ANGEL SCRIPT TEST

    // dlsym test
    // Script *(*create_obj)() = (Script * (*)()) dlsym(dlopen("../sandbox/script.so", RTLD_LAZY), "create_script");
    // Script *script = create_obj();
    // script->Start();

    HyperScript::ScriptEngine SE;
    // SE.LoadScript("test", "../assets/scripts/test.hyper");
    // SE.JustRunTheScript(SE.GetModule("test"));

    Window window(1280, 720, "Vault Engine");
    Statistics::SetStats();

    Shader shader("../shaders/default.glsl");
    Shader skybox_shader("../shaders/skybox.glsl");
    Shader shadow_map_shader("../shaders/shadow_map.glsl");
    Shader shadow_cubemap_shader("../shaders/shadow_map_point.glsl");
    Font::InitFT();
    default_shader = &shader;

    Skybox skybox;

    shader.Bind();

    float rotation = 0.5f;
    double previousTime = glfwGetTime();

    using namespace Engine;
    auto scene = Scene::New("../assets/main.vault");
    Scene::SetMainScene(scene);
    Scene::MakeSceneCamera();
    Scene::Main->SetMainCameraObject(Scene::StaticGameObjects.back(), true);
    Scene::Main->main_camera_object->far = 550;

    // auto gameObject = GameObject::New("My GameObject");
    // gameObject->AddComponent<Components::MeshRenderer>();
    // gameObject->AddComponent<Components::AmbientLight>();
    // auto &meshRenderer = gameObject->GetComponent<Components::MeshRenderer>();
    // gameObject->GetComponent<Components::AmbientLight>().amount = 0.2f;
    // meshRenderer.SetMeshType(Components::MESH_PLANE);
    // meshRenderer.mesh->material.SetDiffuse("../assets/diffuse.png");
    // meshRenderer.mesh->material.SetSpecular("../assets/diffuse.png");
    // meshRenderer.mesh->material.SetNormal("../assets/normal.png");
    // meshRenderer.mesh->material.SetHeight("../assets/displacement.png");

    // auto lightObject = GameObject::New("PointLight");
    // lightObject->AddComponent<Components::PointLight>();
    // lightObject->AddComponent<Components::MeshRenderer>();
    // lightObject->GetComponent<Components::MeshRenderer>().SetMeshType(Components::MESH_PYRAMID);
    // lightObject->GetComponent<Components::PointLight>().enable_shadow_mapping = true;

    // auto emptyObject = GameObject::New("Text");
    // emptyObject->AddComponent<Text3D>();
    // emptyObject->GetComponent<Text3D>().text = "Hello, World!";
    // emptyObject->GetComponent<Text3D>().scale = 0.02;
    // emptyObject->GetComponent<Text3D>().ChangeFont("../assets/fonts/OpenSans-Bold.ttf");
    // auto &empty_transform = emptyObject->GetComponent<Transform>();

    // using namespace Engine::Components;
    // auto &transform = gameObject->GetComponent<Transform>();
    // auto &light_transform = lightObject->GetComponent<Transform>();
    // // transform.rotation.x = glm::radians(90.f);
    // transform.scale = glm::vec3(10, 10, 10);

    // emptyObject->AddChild("Cunt");

    ShadowMap shadow_map;
    shadow_map.ortho_size = 20.0f;

    Runtime runtime(default_shader);
    runtime.shadowMap = &shadow_map;
    EditorLayer editor;

    GUI::LogInfo("Hello, World!");
    GUI::framebufferTextureID = window.framebuffer->framebuffer->texture;
    window.use_imgui_size = true;

    // Model m_2fort("../assets/2fort/2fort.obj")
    // 1model("../assets/capsule.obj");

    // Serializer::Deserialize("../assets/scene.vault");
    // Serializer::DeserializeMaterial("../assets/main.material", GameObject::FindGameObjectByName("My GameObject")->GetComponent<MeshRenderer>().mesh->material);
    // sceneSerializer.Serialize("../assets/scene.vault");

    // Scene::SetMainScene(new_scene);
    // Scene::MakeSceneCamera();
    // Scene::Main->SetMainCameraObject(Scene::StaticGameObjects.back());

    // -- SKELETAL ANIMATION EXAMPLE --
    // Model model("../assets/Crouch To Stand.fbx");
    // Animation danceAnimation("../assets/Crouch To Stand.fbx", &model);
    // Animator animator(&danceAnimation);
    // animator.Update(timestep);
    // auto transforms = animator.GetFinalBoneMatrices();
    // shader.Bind();
    // for (int i = 0; i < transforms.size(); ++i)
    //     shader.SetUniformMat4(("finalBonesMatrices[" + std::to_string(i) + "]").c_str(), transforms[i]);
    // -- SKELETAL ANIMATION EXAMPLE --

    auto Function_GUI = [&] {
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
        editor.GUI();

        for (auto &pointer : GameObject::scheduled_deletions) {
            pointer->UNSAFE_DeleteGameObject();
        }
        GameObject::scheduled_deletions.clear();
    };

    auto Function_ShadowMapRendering = [&] {
        // Directional Light  Shadow Mapping
        if (Scene::Main->EntityRegistry.valid(Scene::Main->EntityRegistry.view<DirectionalLight>().back())) {
            if (Scene::Main->EntityRegistry.get<DirectionalLight>(Scene::Main->EntityRegistry.view<DirectionalLight>().back()).enable_shadow_mapping) {
                shadow_map.RenderSpace([&](std::unique_ptr<Shader> &shadow_shader) {
                    // NOTE: shadow_shader is already binded
                    runtime.DrawToShadowMap(shadow_map.GetDepthBuffer(), *shadow_shader);
                });
            }
        }

        // Point Light Shadow Mapping
        auto pointLightView = Scene::Main->EntityRegistry.view<PointLight>();

        for (auto e : pointLightView) {
            auto &light = Scene::Main->EntityRegistry.get<PointLight>(e);
            if (!light.enable_shadow_mapping)
                continue;

            light.DrawToShadowMap(shadow_cubemap_shader);
        }
    };

    /*
    FUNCTION EXECUTION ORDER:
    1. Shadow Map Rendering Function, the last argument in window.Run
    2. Runtime Function, the first argument in window.Run
    3. GUI Function, the second argument in window.Run

    basically:
    shadow_function()
    runtime_function()
    gui_function()
    */
    float timestep = 0;
    window.Run([&] {
        static double lastTime = 0;
        double now = glfwGetTime();
        timestep = now - lastTime;
        lastTime = now;
        // Update the Main Camera of a scene
        runtime.UpdateMainCamera(window);
        // Reset the Statistic Draw Calls
        Statistics::ResetDrawCalls();

        // Set the background color to RED
        window.SetClearColor(0xFF0000);

        // animator.UpdateAnimation(timestep);

        // Render the skybox
        if (Scene::Main->main_camera_object) {
            skybox.Render(skybox_shader, Scene::Main->main_camera_object->transform->position, Scene::Main->main_camera_object->transform->rotation, Scene::Main->main_camera_object->up);
        }

        // Shadow shenanigans and fuckery
        runtime.ShadowShenanigans(shadow_map);

        // Drawing Meshes and updaing GameObject Components
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);

        // Bind all the camera matricies to the Font Shader (Text)
        Scene::Main->main_camera_object->BindToShader(*Font::font_shader);

        // Update all the GameObjects components
        // shader.Bind();

        runtime.UpdateGameObjects(window); //

        // Scheduling
        if (Serializer::scheduled_scene_path != "") {
            Serializer::Deserialize(Serializer::scheduled_scene_path);
            Serializer::scheduled_scene_path = "";
        } // clang-format off
    }, Function_GUI, Function_ShadowMapRendering);
    // clang-format on

    return 0;
}