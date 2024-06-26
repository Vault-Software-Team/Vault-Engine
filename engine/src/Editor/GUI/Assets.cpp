#include "Engine/Components/MeshRenderer.hpp"
#include "Engine/Components/ModelAnimator.hpp"
#include <Editor/GUI/MainGUI.hpp>
#include <Engine/Scene.hpp>
#include <Engine/GameObject.hpp>
#include <Engine/Components/IncludeComponents.hpp>
#include <icons/icons.h>
#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include <filesystem>
#include <iostream>
#include <Engine/SceneSerialization.hpp>
#include <Engine/Model.hpp>
#include <fstream>
#include <set>

using namespace Engine;
using namespace Engine::Components;
namespace fs = std::filesystem;

static std::string AddFile_Folder = "./assets";

void DirectoryIterator(const std::string &str, const char *filter_str) {
    auto iter = std::filesystem::directory_iterator(str);

    std::string filterStr = filter_str;
    std::transform(filterStr.begin(), filterStr.end(), filterStr.begin(), Editor::asciitolower);

    ImVec2 win_size = ImGui::GetWindowSize();
    ImVec2 cursor_pos = ImGui::GetCursorPos();
    ImGui::Dummy(ImGui::GetContentRegionAvail());
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("gameobject")) {
            const std::string id((char *)payload->Data);
            auto &gameObject = GameObject::FindGameObjectByID(id);

            if (!gameObject) goto skip_if;

            Serializer::CreatePrefab(std::string("./") + gameObject->name + ".prefab", gameObject);
        }

    skip_if:
        ImGui::EndDragDropTarget();
    }
    ImGui::SetCursorPos(cursor_pos);

    std::set<std::filesystem::directory_entry> sorted_by_name;

    for (auto &entry : iter)
        sorted_by_name.insert(entry);

    static std::string FilePath = "";
    static std::string FileName = "";
    static std::string FileDir = "";
    const int PopupElementWidth = 150;

    if (ImGui::BeginPopup("AssetsFile_ContextMenu")) {
        ImGui::PushItemWidth(PopupElementWidth);
        ImGui::InputText("##AssetsFileChangeName", &FileName);
        if (ImGui::Button("Rename File", ImVec2(PopupElementWidth, 0))) {
            fs::rename(FilePath, FileDir + "/" + FileName);
            ImGui::CloseCurrentPopup();
            FilePath = "";
            FileName = "";
        }

        if (ImGui::Button(ICON_FA_TRASH_CAN " Delete File", ImVec2(PopupElementWidth, 0))) {
            fs::remove_all(FilePath);
            ImGui::CloseCurrentPopup();
            FilePath = "";
            FileName = "";
        }

        ImGui::EndPopup();
    }

    for (auto &dir : sorted_by_name) {
        if (dir.path().string().find("VAULT_API", 0) != std::string::npos) {
            continue;
        }

        if (dir.path().string().find("VAULT_OUT", 0) != std::string::npos) {
            continue;
        }

        if (dir.path().string().find("csharp-lib.csproj", 0) != std::string::npos) {
            continue;
        }

        if (dir.is_directory()) {
            bool treenode = ImGui::TreeNode((std::string(ICON_FA_FOLDER) + " " + dir.path().filename().string()).c_str());
            if (treenode) {
                if (ImGui::IsMouseClicked(1) && ImGui::IsItemHovered()) {
                    AddFile_Folder = dir.path().string();
                    ImGui::OpenPopup("Asset_AddFilePopup");
                }

                DirectoryIterator(dir.path().string(), filter_str);
                ImGui::TreePop();
            } else {
                if (ImGui::IsMouseClicked(1) && ImGui::IsItemHovered()) {
                    AddFile_Folder = dir.path().string();
                    ImGui::OpenPopup("Asset_AddFilePopup");
                }
            }

        } else {
            std::string dir_filename = dir.path().filename().string();
            std::transform(dir_filename.begin(), dir_filename.end(), dir_filename.begin(), Editor::asciitolower);
            if (dir_filename.find(filterStr) == std::string::npos && filterStr != "")
                continue;
            std::string icon = ICON_FA_FILE;
            std::string name = dir.path().filename().string();

            if (name.ends_with(".ttf") || name.ends_with(".otf")) {
                icon = ICON_FA_FONT;
            }
            if (name.ends_with(".png") || name.ends_with(".tga") || name.ends_with(".tiff") || name.ends_with(".jpg") || name.ends_with(".jpeg")) {
                icon = ICON_FA_IMAGE;
            }
            if (name.ends_with(".obj") || name.ends_with(".blender") || name.ends_with(".blend") || name.ends_with(".dae") || name.ends_with(".gltf") || name.ends_with(".glTF") || name.ends_with(".fbx")) {
                icon = ICON_FA_CUBES;
            }
            if (name.ends_with(".mtl") || name.ends_with(".material")) {
                icon = ICON_FA_PAINT_ROLLER;
            }
            if (name.ends_with(".vault")) {
                icon = ICON_FA_CUBES_STACKED;
            }
            if (name.ends_with(".hyper") || name.ends_with(".cs")) {
                icon = ICON_FA_CODE;
            }
            if (name.ends_with(".prefab")) {
                icon = ICON_FA_CUBE;
            }

            bool pressed = ImGui::Selectable((icon + " " + dir.path().filename().string()).c_str());

            if (ImGui::IsMouseClicked(1) && ImGui::IsItemHovered()) {
                FilePath = dir.path().string();
                FileName = dir.path().filename().string();
                FileDir = dir.path().parent_path().string();
                ImGui::OpenPopup("AssetsFile_ContextMenu");
            }

            if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered()) {
                if (name.ends_with(".vault")) {
                    Editor::GUI::selected_gameObject = nullptr;
                    Serializer::scheduled_scene_path = dir.path().string();
                }

                if (name.ends_with(".prefab")) {
                    Serializer::LoadPrefab(dir.path().string());
                }

                if (name.ends_with(".cs") || name.ends_with(".hyper")) {
                    std::ifstream stream(dir.path().string());
                    std::stringstream ss;
                    ss << stream.rdbuf();
                    Editor::GUI::TextEditor_text = ss.str();
                    Editor::GUI::TextEditor_path = dir.path().string();
                    Editor::GUI::text_editor->SetText(Editor::GUI::TextEditor_text);
                    stream.close();
                }

                if (name.ends_with(".obj") || name.ends_with(".blender") || name.ends_with(".blend") || name.ends_with(".dae") || name.ends_with(".gltf") || name.ends_with(".glTF") || name.ends_with(".fbx")) {
                    Engine::ModelMesh *model = new Engine::ModelMesh(dir.path().string());

                    auto &gameObject = GameObject::New("Model");

                    int i = 0;
                    for (auto &mesh : model->meshes) {
                        auto &_gameObject = GameObject::New(mesh.name);
                        _gameObject->AddComponent<MeshRenderer>();
                        _gameObject->parent = gameObject->ID;

                        auto &meshRenderer = _gameObject->GetComponent<MeshRenderer>();
                        meshRenderer.SetCustomMeshType(mesh.vertices, mesh.indices);
                        meshRenderer.mesh_index = i;
                        meshRenderer.mesh_path = model->path;
                        meshRenderer.model = std::shared_ptr<ModelMesh>(model);

                        i++;
                    }
                    auto &comp = gameObject->AddComponent<ModelAnimator>();
                    comp.model = std::make_unique<ModelMesh>(dir.path().string());
                    // Engine::Model model(dir.path().string());
                }
            }

            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                Editor::GUI::dragPayload = fs::absolute(dir.path()).string();
                auto length = dir.path().string().length();
                if (name.ends_with(".material")) {
                    ImGui::SetDragDropPayload("material", dir.path().string().c_str(), length + 1);
                } else if (name.ends_with(".ttf") || name.ends_with(".otf")) {
                    ImGui::SetDragDropPayload("font_file", dir.path().string().c_str(), length + 1);
                } else if (name.ends_with(".png") || name.ends_with(".tga") || name.ends_with(".tiff") || name.ends_with(".jpg") || name.ends_with(".jpeg")) {
                    ImGui::SetDragDropPayload("image_file", dir.path().string().c_str(), length + 1);
                } else if (name.ends_with(".vault")) {
                    ImGui::SetDragDropPayload("scene_file", dir.path().string().c_str(), length + 1);
                } else if (name.ends_with(".obj") || name.ends_with(".blender") || name.ends_with(".blend") || name.ends_with(".dae") || name.ends_with(".gltf") || name.ends_with(".glTF") || name.ends_with(".fbx")) {
                    ImGui::SetDragDropPayload("model_file", dir.path().string().c_str(), length + 1);
                } else if (name.ends_with(".hyper")) {
                    ImGui::SetDragDropPayload("hyperscript_file", dir.path().string().c_str(), length + 1);
                } else if (name.ends_with(".prefab")) {
                    ImGui::SetDragDropPayload("prefab", dir.path().string().c_str(), length + 1);
                }
                ImGui::Text("%s %s", icon.c_str(), dir.path().filename().string().c_str());
                ImGui::EndDragDropSource();
            }

            if ((name.ends_with(".mtl") || name.ends_with(".material")) && (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))) {
                Editor::GUI::MaterialInspector_material_path = dir.path().relative_path().string();
                Editor::GUI::isMaterialInspectorOpen = true;

                Serializer::DeserializeMaterial(dir.path().relative_path().string(), Editor::GUI::MaterialInspector_material);
            }
        }
    }
}

namespace Editor {
    void GUI::Assets() {
        constexpr int filter_str_size = 256;
        static char filter_str[filter_str_size];
        static bool AddFilePopup = false;

        ImGui::Begin(ICON_FA_FOLDER " Assets");
        ImGui::PushItemWidth(ImGui::GetWindowSize().x - 20);
        ImGui::InputText("##AssetsFilterString", filter_str, filter_str_size);

        if (ImGui::Button(ICON_FA_PLUS " Add File", ImVec2(ImGui::GetWindowSize().x - 20, 0))) {
            ImGui::OpenPopup("Asset_AddFilePopup");
        }

        if (ImGui::BeginPopup("Asset_AddFilePopup")) {
            static std::string Filename = "New File";
            ImGui::PushItemWidth(150);
            ImGui::InputText("##AssetsAddFileString", &Filename);
            ImGui::TextWrapped("%s", AddFile_Folder.c_str());

            if (AddFile_Folder != "./assets") {
                if (ImGui::Button(ICON_FA_TRASH_CAN " Delete Folder", ImVec2(150, 0))) {
                    fs::remove_all(AddFile_Folder);
                    ImGui::CloseCurrentPopup();
                    AddFile_Folder = "";
                    Filename = "New File";
                }
            }

            if (ImGui::Button(ICON_FA_CODE " Add C# Script", ImVec2(150, 0))) {
                std::ofstream ofs(AddFile_Folder + "/" + Filename + ".cs");

                ofs << "using Vault;\n";
                ofs << "public class CameraTest : Entity {\n";
                ofs << "void OnInit(string ID) {\n";
                ofs << "    SetObjectID(ID);\n";
                ofs << "}\n\n";

                ofs << "private void OnStart(string ID) {\n";
                ofs << "}\n\n";

                ofs << "private void OnUpdate() {\n";
                ofs << "}\n";
                ofs << "\n";
                ofs << "void OnMouseEnter() {\n";
                ofs << "    Debug.Log(\"Mouse Entered!\");\n";
                ofs << "}\n\n";

                ofs << "void OnMouseExit() {\n";
                ofs << "    Debug.Log(\"Mouse Left!\");\n";
                ofs << "}\n";
                ofs << "}\n";

                ofs.close();
                ImGui::CloseCurrentPopup();
                AddFile_Folder = "./assets";
                Filename = "New File";
            }

            if (ImGui::Button(ICON_FA_PAINT_ROLLER " Add PBR Shader", ImVec2(150, 0))) {
                std::ifstream stream("./shaders/pbr.glsl");
                std::stringstream ss;
                ss << stream.rdbuf();
                std::string content = ss.str();

                std::ofstream ofs(AddFile_Folder + "/" + Filename + "_pbr.glsl");
                ofs << content << "\n";
                ofs.close();
                ImGui::CloseCurrentPopup();
                AddFile_Folder = "./assets";
                Filename = "New File";
            }

            if (ImGui::Button(ICON_FA_PAINT_ROLLER " Add GLSL Shader", ImVec2(150, 0))) {
                std::ifstream stream("./shaders/default.glsl");
                std::stringstream ss;
                ss << stream.rdbuf();
                std::string content = ss.str();

                std::ofstream ofs(AddFile_Folder + "/" + Filename + ".glsl");
                ofs << content << "\n";
                ofs.close();
                ImGui::CloseCurrentPopup();
                AddFile_Folder = "./assets";
                Filename = "New File";
            }

            if (ImGui::Button(ICON_FA_PAINT_ROLLER " Add Text File", ImVec2(150, 0))) {
                std::ofstream ofs(AddFile_Folder + "/" + Filename + ".txt");
                ofs.close();
                ImGui::CloseCurrentPopup();
                AddFile_Folder = "./assets";
                Filename = "New File";
            }

            ImGui::EndPopup();
        } else {
            AddFile_Folder = "./assets";
        }

        std::string icon = ICON_FA_CUBE;

        DirectoryIterator("./assets", filter_str);

        ImGui::End();
    }
} // namespace Editor