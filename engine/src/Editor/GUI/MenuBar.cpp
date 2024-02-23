#include "Engine/Mono/CSharp.hpp"
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

    static void SaveSceneAs() {
        if (ImGuiFileDialog::Instance()->Display("SaveSceneAsDialog")) {
            // action if OK
            if (ImGuiFileDialog::Instance()->IsOk()) {
                std::string filePathName =
                    ImGuiFileDialog::Instance()->GetFilePathName();
                // remove cwd from filePathName
                std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

                if (std::filesystem::exists(std::filesystem::path(filePath))) {
                    Serializer::Serialize(Scene::Main, filePath);
                }
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
                        Serializer::Serialize(Scene::Main, "../assets/" + uuid::generate_uuid_v4() + ".vault");
                    }
                }

                if (ImGui::MenuItem("Save Scene As...")) {
                    ImGuiFileDialog::Instance()->OpenDialog("SaveSceneAsDialog", "Save Scene As...", ".vault", ".");
                }

                if (ImGui::MenuItem("Open Scene")) {
                    if (std::filesystem::exists(std::filesystem::path(Scene::Main->scene_file_path))) {
                        Serializer::Serialize(Scene::Main, Scene::Main->scene_file_path);
                    } else {
                        Serializer::Serialize(Scene::Main, "../assets/" + uuid::generate_uuid_v4() + ".vault");
                    }
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
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("C#")) {
                if (ImGui::MenuItem("Reload Assembly")) {
                    CSharp::instance->ReloadAssembly();
                }
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        ShadowMapConfiguration();
    }
} // namespace Editor