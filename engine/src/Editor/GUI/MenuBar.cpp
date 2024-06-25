#include "Editor/EditorLayer.hpp"
#include "Engine/Model.hpp"
#include "Engine/Mono/CSharp.hpp"
#include "Renderer/Shader.hpp"
#include "imgui/ImGuiNotify.hpp"
#include <Editor/GUI/MainGUI.hpp>
#include <Engine/Scene.hpp>
#include <Engine/GameObject.hpp>
#include <Engine/Components/IncludeComponents.hpp>
#include <icons/icons.h>
#include <imgui/imgui.h>
#include <Engine/SceneSerialization.hpp>
#include <filesystem>
#include <uuid.hpp>
#include <Engine/Runtime.hpp>
#include <ImGuiFileDialog/ImGuiFileDialog.h>

using namespace Engine;
using namespace Engine::Components;

namespace Editor {
    static bool open = false;
    static bool open_hdr = false;
    static bool open_shader_debug = false;
    static bool open_editor = false;
    static bool open_model_debug = false;

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
            ImGui::DragFloat("(HDR) Exposure", &Serializer::config.HDR.exposure);
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

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Renderer")) {
                if (ImGui::BeginMenu("Shadows")) {
                    if (ImGui::MenuItem("Configuration")) {
                        open = true;
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Renderer")) {
                    if (ImGui::MenuItem("Configuration")) {
                        open_hdr = true;
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

        ShadowMapConfiguration();
        HDRConfiguration();
        ShaderDebug();
        EditorConfig();
        ModelDebug();
    }
} // namespace Editor