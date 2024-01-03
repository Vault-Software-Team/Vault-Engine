#include <Editor/GUI/MainGUI.hpp>
#include <Engine/Scene.hpp>
#include <Engine/GameObject.hpp>
#include <Engine/Components/IncludeComponents.hpp>
#include <icons/icons.h>
#include <imgui/imgui.h>
#include <Engine/SceneSerialization.hpp>
#include <filesystem>
#include <uuid.hpp>

using namespace Engine;
using namespace Engine::Components;

namespace Editor {
    void GUI::MenuBar() {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Save Scene")) {
                    if (std::filesystem::exists(std::filesystem::path(Scene::Main->scene_file_path))) {
                        Serializer::Serialize(Scene::Main, Scene::Main->scene_file_path);
                    } else {
                        Serializer::Serialize(Scene::Main, "../assets/" + uuid::generate_uuid_v4() + ".vault");
                    }
                }
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
    }
} // namespace Editor