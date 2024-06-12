#pragma once
#include "imgui/ImGuiNotify.hpp"
#include <Editor/GUI/MainGUI.hpp>
#include <Engine/Scene.hpp>
#include <Engine/GameObject.hpp>
#include <Engine/Components/IncludeComponents.hpp>
#include <icons/icons.h>
#include <imgui/imgui.h>
#include <filesystem>
#include <iostream>
#include <ImGuiFileDialog/ImGuiFileDialog.h>
#include <Engine/SceneSerialization.hpp>

namespace Editor {
    DLL_API bool GUI::isConfigurationOpen = false;
    void GUI::Config() {
        if (!isConfigurationOpen) return;

        ImGui::SetNextWindowSizeConstraints(ImVec2(50.0f, 50.0f), ImVec2(500.0f, 500.f));
        if (ImGui::Begin(ICON_FA_GEAR " Configuration", &isConfigurationOpen, ImGuiWindowFlags_NoDocking)) {
            ImGui::Text("Main Scene (Drag Here): %s", Engine::Serializer::config.main_scene.c_str());
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("scene_file")) {
                    Engine::Serializer::config.main_scene = (char *)payload->Data;
                }
            }

            ImGui::InputText("Game Title", &Engine::Serializer::config.title);

            if (ImGui::Button(ICON_FA_FLOPPY_DISK " Save Configuration")) {
                Engine::Serializer::SaveConfigFile("./assets/config.yaml");
                ImGui::InsertNotification({ImGuiToastType::Success, 3000, "Saved configuration successfully."});
            }
            ImGui::NewLine();

            ImGui::End();
        }
    }
} // namespace Editor