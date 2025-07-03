#include "Editor/EditorLayer.hpp"
#include "Engine/Model.hpp"
#include "Engine/Mono/CSharp.hpp"
#include "Engine/PostProcessing.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/Window.hpp"
#include "imgui/ImGuiNotify.hpp"
#include <Editor/GUI/MainGUI.hpp>
#include <Engine/Scene.hpp>
#include <Engine/GameObject.hpp>
#include <Engine/Components/IncludeComponents.hpp>
#include <exception>
#include <icons/icons.h>
#include <imgui/imgui.h>
#include <Engine/SceneSerialization.hpp>
#include <filesystem>
#include <uuid.hpp>
#include <Engine/Runtime.hpp>
#include <ImGuiFileDialog/ImGuiFileDialog.h>

using namespace Engine;
using namespace Engine::Components;

namespace fs = std::filesystem;

namespace Editor {
    static bool open = false;
    static bool open_hdr = false;
    static bool open_shader_debug = false;
    static bool open_editor = false;
    static bool open_model_debug = false;
    static bool open_post_processing = false;
    static bool open_pbr_config = false;

    static ImVec4 RGB255ToRGB1(float r, float g, float b, float a) {
        return ImVec4(r / 255, g / 255, b / 255, a / 255);
    }

    static void ShadowMapConfiguration() {
        if (!open)
            return;

        if (ImGui::Begin("Shadow Mapping", &open, ImGuiWindowFlags_NoDocking)) {
            VaultRenderer::ShadowMap *map = Engine::Runtime::instance->shadowMap;
            ImGui::DragFloat("Near", &map->near);
            ImGui::DragFloat("Far", &map->far);
            ImGui::DragFloat("Ortho Size", &map->ortho_size);
            ImGui::ImageButton((void *)map->GetTextureID(), ImVec2(200, 200), ImVec2(0, 1), ImVec2(1, 0));

            ImGui::End();
        }
    }

    static void HDRConfiguration() {
        if (!open_hdr) return;

        if (ImGui::Begin("Renderer Config", &open_hdr, ImGuiWindowFlags_NoDocking)) {
            ImGui::DragFloat("(HDR) Exposure", &Serializer::config.HDR.exposure, 0.1);
            ImGui::Checkbox("Bloom", &VaultRenderer::Window::Renderer.Bloom);
            ImGui::Checkbox("PBR", &Runtime::instance->usePBR);

            ImGui::End();
        }
    }

    static void ShaderDebug() {
        if (!open_shader_debug) return;

        ImGui::SetNextWindowSizeConstraints(ImVec2(250.0f, 250.0f), ImVec2(1000.0f, 1000.f));
        if (ImGui::Begin("Shader List", &open_shader_debug, ImGuiWindowFlags_NoDocking)) {
            for (auto &sMap : VaultRenderer::Shader::shaders) {
                auto *shader = sMap.second;
                std::filesystem::path p(shader->path);

                ImGui::PushStyleColor(ImGuiCol_Text, shader->EngineShader ? ImVec4((float)89 / 255, (float)189 / 255, 1, 1) : ImVec4(1, (float)168 / 255, (float)61 / 255, 1));

                if (ImGui::TreeNodeEx(shader->path.c_str(), ImGuiTreeNodeFlags_SpanAvailWidth, "%s", p.filename().string().c_str())) {
                    ImGui::PopStyleColor();

                    ImVec4 color = shader->EngineShader ? ImVec4((float)89 / 255, (float)189 / 255, 1, 1) : ImVec4(1, (float)168 / 255, (float)61 / 255, 1);
                    ImGui::TextColored(color, shader->EngineShader ? "Engine Required Shader" : "User Loaded Shader");

                    for (auto type : shader->types) {
                        switch (type) {
                        case VaultRenderer::Shader::VERTEX: {
                            ImGui::Text("Has Vertex Shader");

                            break;
                        }
                        case VaultRenderer::Shader::FRAGMENT: {
                            ImGui::Text("Has Fragment Shader");
                            break;
                        }
                        case VaultRenderer::Shader::GEOMETRY: {
                            ImGui::Text("Has Geometry Shader");
                            break;
                        }
                        }
                    }

                    ImGui::PushStyleColor(ImGuiCol_Text, RGB255ToRGB1(169, 64, 255, 255));
                    if (ImGui::TreeNodeEx("SHADER_DEBUG_UNIFORMS", ImGuiTreeNodeFlags_SpanAvailWidth, "Uniforms")) {
                        ImGui::PopStyleColor();

                        for (auto &uniform : shader->active_uni_names) {
                            ImGui::Text("%s", uniform.c_str());
                        }

                        ImGui::TreePop();
                    } else
                        ImGui::PopStyleColor();

                    ImGui::PushStyleColor(ImGuiCol_Text, RGB255ToRGB1(255, 99, 216, 255));
                    if (ImGui::TreeNodeEx("SHADER_DEBUG_ATTRIBS", ImGuiTreeNodeFlags_SpanAvailWidth, "Attributes")) {
                        ImGui::PopStyleColor();

                        for (auto &attrib : shader->active_attrib_names) {
                            ImGui::Text("%s", attrib.c_str());
                        }

                        ImGui::TreePop();
                    } else
                        ImGui::PopStyleColor();

                    ImGui::TreePop();
                } else
                    ImGui::PopStyleColor();
            }

            ImGui::End();
        }
    }

    static void ModelDebug() {
        if (!open_model_debug) return;

        ImGui::SetNextWindowSizeConstraints(ImVec2(250.0f, 250.0f), ImVec2(1000.0f, 1000.f));
        if (ImGui::Begin("Model List", &open_model_debug, ImGuiWindowFlags_NoDocking)) {
            for (auto &mMap : ModelMesh::LoadedModels) {
                auto &info = mMap.second;
                std::filesystem::path p(info.ref->path);

                ImGui::Text("%s", info.ref->path.c_str());
            }

            ImGui::End();
        }
    }

    static void EditorConfig() {
        if (!open_editor) return;

        if (ImGui::Begin(ICON_FA_GEAR " Editor Config", &open_editor, ImGuiWindowFlags_NoDocking)) {
            ImGui::Checkbox("Show Icons", &EditorLayer::instance->EnableIconGizmo);
            ImGui::Checkbox("Show Collider Gizmo", &EditorLayer::instance->EnableColliderGizmo);

            ImGui::End();
        }
    }

    static void PostProcessingConfig() {
        if (!open_post_processing) return;

        if (ImGui::Begin(ICON_FA_GEAR " Post Processing Config", &open_post_processing, ImGuiWindowFlags_NoDocking)) {
            ImGui::Checkbox("Global Bloom", &PostProcessing::GlobalBloom);
            ImGui::DragFloat("Bloom Threshold", &PostProcessing::BloomThreshold, 0.01, 0.0f);
            ImGui::DragFloat("Shadow Strength", &PostProcessing::ShadowStrength, 0.01, 0.0f);
            GUI::DrawVec3Control("Bloom Multiplier", PostProcessing::BloomMultiplier, 1.0f);

            ImGui::End();
        }
    }

    static void PBRConfig() {
        if (!open_pbr_config) return;

        if (ImGui::Begin(ICON_FA_GEAR " PBR Config", &open_pbr_config, ImGuiWindowFlags_NoDocking)) {
            ImGui::Checkbox("Image Based Lightning", &PostProcessing::PBR_IBL);

            ImGui::End();
        }
    }

    static void SaveSceneAs() {
        if (ImGuiFileDialog::Instance()->Display("SaveSceneAsDialog")) {
            // action if OK
            if (ImGuiFileDialog::Instance()->IsOk()) {
                std::string filePathName =
                    ImGuiFileDialog::Instance()->GetFilePathName();
                // remove cwd from filePathName
                std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

                Scene::Main->scene_file_path = filePathName;
                Serializer::Serialize(Scene::Main, filePathName);
                ImGui::InsertNotification({ImGuiToastType::Success, 3000, "Saved scene as \"%s\"", filePathName.c_str()});
            }

            ImGuiFileDialog::Instance()->Close();
        }
    }

    void GUI::MenuBar() {
        static bool isLinux = false;
        SaveSceneAs();

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Save Scene")) {
                    if (std::filesystem::exists(std::filesystem::path(Scene::Main->scene_file_path))) {
                        Serializer::Serialize(Scene::Main, Scene::Main->scene_file_path);
                    } else {
                        Serializer::Serialize(Scene::Main, "./" + uuid::generate_uuid_v4() + ".vault");
                    }

                    ImGui::InsertNotification({ImGuiToastType::Success, 3000, "Saved scene \"%s\"", Scene::Main->scene_file_path.c_str()});
                }

                if (ImGui::MenuItem("Save Scene As...")) {
                    ImGuiFileDialog::Instance()->OpenDialog("SaveSceneAsDialog", "Save Scene As...", ".vault", ".");
                }

                if (ImGui::MenuItem("Config")) {
                    Editor::GUI::isConfigurationOpen = true;
                }

                if (ImGui::BeginMenu("Build Game")) {
                    if (ImGui::MenuItem("Linux")) {
                        isLinux = true;
                        ImGuiFileDialog::Instance()->OpenDialog("BuildGameSelectPath", "Build for Linux", nullptr, ".");
                    }

                    if (ImGui::MenuItem("Windows")) {
                        isLinux = false;
                        ImGuiFileDialog::Instance()->OpenDialog("BuildGameSelectPath", "Build for Linux", nullptr, ".");
                    }

                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Renderer")) {
                if (ImGui::MenuItem("Configuration")) {
                    open_hdr = true;
                }

                if (ImGui::MenuItem("Render To Image")) {
                    VaultRenderer::Window::window->render_to_image = true;
                    VaultRenderer::Window::window->WindowSizeBeforeImageRender.x = VaultRenderer::Window::window->width;
                    VaultRenderer::Window::window->WindowSizeBeforeImageRender.y = VaultRenderer::Window::window->height;
                }

                if (ImGui::BeginMenu("Post Processing")) {
                    if (ImGui::MenuItem("Configuration")) {
                        open_post_processing = true;
                    }
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("PBR")) {
                    if (ImGui::MenuItem("Configuration")) {
                        open_pbr_config = true;
                    }
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Shadows")) {
                    if (ImGui::MenuItem("Configuration")) {
                        open = true;
                    }
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Lists")) {
                    if (ImGui::MenuItem("Shaders")) {
                        open_shader_debug = true;
                    }

                    if (ImGui::MenuItem("Models")) {
                        open_model_debug = true;
                    }

                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Editor")) {
                if (ImGui::MenuItem("Settings")) {
                    open_editor = true;
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("C#")) {
                if (ImGui::MenuItem("Reload Assembly")) {
                    CSharp::instance->ReloadAssembly();
                }

                if (ImGui::MenuItem("Compile Assembly")) {
                    CSharp::instance->CompileAssemblies();
                }
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        if (ImGuiFileDialog::Instance()->Display("BuildGameSelectPath")) {
            // action if OK
            if (ImGuiFileDialog::Instance()->IsOk()) {
                fs::path build_path = ImGuiFileDialog::Instance()->GetCurrentPath();

                if (fs::equivalent(build_path, fs::path("./"))) {
                    ImGui::InsertNotification({ImGuiToastType::Error, 5000, "Cannot build in the same directory as the project."});
                    ImGuiFileDialog::Instance()->Close();
                    return;
                }

                try {
                    if (fs::exists(build_path / "BUILD_GAME")) fs::remove_all(build_path / "BUILD_GAME");
                    fs::create_directory(build_path / "BUILD_GAME");
                    fs::create_directory(build_path / "BUILD_GAME" / "bin");

                    fs::copy("./shaders", build_path / "BUILD_GAME" / "shaders", fs::copy_options::recursive);
                    fs::copy("./editor", build_path / "BUILD_GAME" / "editor", fs::copy_options::recursive);
                    fs::copy("./fonts", build_path / "BUILD_GAME" / "fonts", fs::copy_options::recursive);
                    fs::copy("./mono", build_path / "BUILD_GAME" / "mono", fs::copy_options::recursive);
                    fs::copy("./assets", build_path / "BUILD_GAME" / "assets", fs::copy_options::recursive);
                    fs::copy(isLinux ? "./lib" : "./dlls", build_path / "BUILD_GAME" / (isLinux ? "lib" : "dlls"), fs::copy_options::recursive);
                    fs::copy("./default_models", build_path / "BUILD_GAME" / "default_models", fs::copy_options::recursive);
                    fs::copy(isLinux ? "./bin/gamebin" : "./bin/game.exe", build_path / "BUILD_GAME" / "bin" / (isLinux ? "gamebin" : "game.exe"), fs::copy_options::recursive);
                    fs::copy(isLinux ? "./bin/LaunchGame.sh" : "./bin/LaunchGame.bat", build_path / "BUILD_GAME" / (isLinux ? "LaunchGame.sh" : "LaunchGame.bat"), fs::copy_options::recursive);
                    ImGui::InsertNotification({ImGuiToastType::Success, 5000, "Built game for %s successfully!", isLinux ? "linux" : "windows"});
                } catch (std::exception &e) {
                    std::cout << "[FS-ERROR] " << e.what() << "\n";
                    ImGui::InsertNotification({ImGuiToastType::Error, 5000, "%s", e.what()});
                }
            }
            ImGuiFileDialog::Instance()->Close();
        }

        ShadowMapConfiguration();
        HDRConfiguration();
        ShaderDebug();
        EditorConfig();
        ModelDebug();
        PostProcessingConfig();
        PBRConfig();
    }
} // namespace Editor