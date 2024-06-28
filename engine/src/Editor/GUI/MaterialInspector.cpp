#pragma once
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
    DLL_API bool GUI::isMaterialInspectorOpen = false;
    DLL_API VaultRenderer::Material GUI::MaterialInspector_material;
    DLL_API std::string GUI::MaterialInspector_material_path = "";

    void GUI::MaterialInspector() {
        if (!isMaterialInspectorOpen || MaterialInspector_material_path == "")
            return;

        ImGui::SetNextWindowSizeConstraints(ImVec2(50.0f, 50.0f), ImVec2(500.0f, 500.f));
        if (ImGui::Begin(ICON_FA_BRUSH " Material Inspector", &isMaterialInspectorOpen, ImGuiWindowFlags_NoDocking)) {
            ImGui::Text("Path: %s", MaterialInspector_material_path.c_str());
            ImGui::NewLine();

            uint32_t diffuseID = MaterialInspector_material.diffuse == nullptr ? 0 : MaterialInspector_material.diffuse->texture_data->ID;
            uint32_t specularID = MaterialInspector_material.specular == nullptr ? 0 : MaterialInspector_material.specular->texture_data->ID;
            uint32_t normalID = MaterialInspector_material.normal == nullptr ? 0 : MaterialInspector_material.normal->texture_data->ID;
            uint32_t heightID = MaterialInspector_material.height == nullptr ? 0 : MaterialInspector_material.height->texture_data->ID;
            uint32_t metallic_mapID = MaterialInspector_material.metallic_map == nullptr ? 0 : MaterialInspector_material.metallic_map->texture_data->ID;
            uint32_t roughness_mapID = MaterialInspector_material.roughness_map == nullptr ? 0 : MaterialInspector_material.roughness_map->texture_data->ID;
            uint32_t ao_mapID = MaterialInspector_material.ao_map == nullptr ? 0 : MaterialInspector_material.ao_map->texture_data->ID;

            ImGui::Text("Diffuse Texture");
            ImGui::ImageButton((void *)diffuseID, ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
            if (ImGui::IsItemClicked(1)) {
                MaterialInspector_material.diffuse.reset();
                MaterialInspector_material.diffuse = nullptr;
            }
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("image_file")) {
                    std::string path = (char *)payload->Data;
                    std::cout << path << "\n";
                    MaterialInspector_material.SetDiffuse(path);
                }
            }
            ImGui::NewLine();

            ImGui::Text("Specular Texture");
            ImGui::ImageButton((void *)specularID, ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
            if (ImGui::IsItemClicked(1)) {
                MaterialInspector_material.specular.reset();
                MaterialInspector_material.specular = nullptr;
            }
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("image_file")) {
                    std::string path = (char *)payload->Data;
                    MaterialInspector_material.SetSpecular(path);
                }
            }
            ImGui::NewLine();

            ImGui::Text("Normal Texture");
            ImGui::ImageButton((void *)normalID, ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
            if (ImGui::IsItemClicked(1)) {
                MaterialInspector_material.normal.reset();
                MaterialInspector_material.normal = nullptr;
            }
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("image_file")) {
                    std::string path = (char *)payload->Data;
                    MaterialInspector_material.SetNormal(path);
                }
            }
            ImGui::NewLine();

            ImGui::Text("Height Texture");
            ImGui::ImageButton((void *)heightID, ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
            if (ImGui::IsItemClicked(1)) {
                MaterialInspector_material.height.reset();
                MaterialInspector_material.height = nullptr;
            }
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("image_file")) {
                    std::string path = (char *)payload->Data;
                    MaterialInspector_material.SetHeight(path);
                }
            }
            ImGui::NewLine();

            ImGui::Text("Metallic Texture");
            ImGui::ImageButton((void *)metallic_mapID, ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
            if (ImGui::IsItemClicked(1)) {
                MaterialInspector_material.metallic_map.reset();
                MaterialInspector_material.metallic_map = nullptr;
            }
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("image_file")) {
                    std::string path = (char *)payload->Data;
                    MaterialInspector_material.SetMetallic(path);
                }
            }
            ImGui::NewLine();

            ImGui::Text("Roughness Texture");
            ImGui::ImageButton((void *)roughness_mapID, ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
            if (ImGui::IsItemClicked(1)) {
                MaterialInspector_material.roughness_map.reset();
                MaterialInspector_material.roughness_map = nullptr;
            }
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("image_file")) {
                    std::string path = (char *)payload->Data;
                    MaterialInspector_material.SetRoughness(path);
                }
            }
            ImGui::NewLine();

            ImGui::Text("Ambient Occlusion Texture");
            ImGui::ImageButton((void *)ao_mapID, ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
            if (ImGui::IsItemClicked(1)) {
                MaterialInspector_material.ao_map.reset();
                MaterialInspector_material.ao_map = nullptr;
            }
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("image_file")) {
                    std::string path = (char *)payload->Data;
                    MaterialInspector_material.SetAO(path);
                }
            }
            ImGui::NewLine();

            ImGui::ColorEdit4("Color", &MaterialInspector_material.color.x);
            ImGui::ColorEdit4("Emission Color", &MaterialInspector_material.emissionColor.x);
            ImGui::DragFloat("Metallic", &MaterialInspector_material.metallic);
            ImGui::DragFloat("Roughness", &MaterialInspector_material.roughness);
            ImGui::DragFloat("AO", &MaterialInspector_material.ao);
            GUI::DrawVec2Control("Texture UVs", MaterialInspector_material.texUVs);

            if (ImGui::Button(ICON_FA_FLOPPY_DISK " Save")) {
                Engine::Serializer::SerializeMaterial(MaterialInspector_material_path, MaterialInspector_material);
            }
            if (ImGui::Button(ICON_FA_FLOPPY_DISK " Save Material As")) {
                ImGuiFileDialog::Instance()->OpenDialog("MaterialInspector_SaveAs", ICON_FA_FLOPPY_DISK " Save Material As", ".material", ".");
            }

            if (ImGuiFileDialog::Instance()->Display("MaterialInspector_SaveAs")) {
                // action if OK
                if (ImGuiFileDialog::Instance()->IsOk()) {
                    Engine::Serializer::SerializeMaterial(ImGuiFileDialog::Instance()->GetFilePathName(), MaterialInspector_material);
                }

                ImGuiFileDialog::Instance()->Close();
            }

            ImGui::End();
        }
    }
} // namespace Editor