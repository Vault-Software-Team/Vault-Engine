#include "imgui/imgui.h"
#include <Engine/Components/ModelRenderer.hpp>
#include <Editor/GUI/MainGUI.hpp>
#include <icons/icons.h>
#include <Engine/SceneSerialization.hpp>
#include <Engine/Runtime.hpp>
#include <string>

namespace Engine::Components {
    void ModelRenderer::OnGUI() {
        DrawComponent<ModelRenderer>(Scene::Main->EntityRegistry, 40, [&] {
            // ImGui::Button(material_path == "" ? "Drag Material File" : material_path.c_str());
            // if (ImGui::BeginDragDropTarget()) {
            //     if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("material")) {
            //         std::string path = (char *)payload->Data;
            //         material_path = path;
            //         if (mesh)
            //             Serializer::DeserializeMaterial(path, mesh->material);
            //     }
            // }
            //     ImGui::Separator();

            ImGui::Button("Drag Model File");
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("model_file")) {
                    std::string path = (char *)payload->Data;
                    model.reset();
                    model = std::make_unique<ModelMesh>(path);
                }
            }
            if (model) {
                ImGui::Text("%s", model->path.c_str());
            }

            ImGui::Button("Drag Animation File");
            if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(2)) {
                animator.reset();
                animation.reset();
                animation_path = "";
            }
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("model_file")) {
                    std::string path = (char *)payload->Data;
                    animation_path = path;
                    SetAnimation(animation_path);
                    play_animation = true;
                }
            }
            ImGui::Text("%s", animation_path.c_str());
            ImGui::DragFloat("Animation Time Scale", &time_scale, 0.01);
            if (animation_path != "") {
                if (ImGui::Button(play_animation ? "Stop Animation" : "Play Animation")) {
                    play_animation = !play_animation;

                    if (!play_animation && this->model) {
                        SetAnimation(this->model->path);
                    } else {
                        SetAnimation(animation_path);
                    }
                }
            }
            // Mesh Materials
            ImGui::Text("Mesh Materials");
            if (model) {
                if (ImGui::TreeNode(ICON_FA_PAINTBRUSH " Mesh Materials")) {
                    int i = 0;
                    for (auto &mesh : model->meshes) {
                        ImGui::PushID(("ModelRenderer_" + mesh.name + std::to_string(i)).c_str());
                        if (ImGui::TreeNode(mesh.name.c_str())) {
                            ImGui::Button(mesh.material.filePath == "" ? "Drag Material File" : mesh.material.filePath.c_str());
                            if (ImGui::BeginDragDropTarget()) {
                                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("material")) {
                                    mesh.material.filePath = (char *)payload->Data;
                                    Serializer::DeserializeMaterial(mesh.material.filePath, mesh.material);
                                }
                            }
                            ImGui::TreePop();
                        }
                        ImGui::PopID();
                        i++;
                    }

                    ImGui::TreePop();
                }
            }

            // if (animation && animator) {
            //     bool bonemap_treenode = ImGui::TreeNode("Bone Transforms");

            //     if (bonemap_treenode) {
            //         auto &boneInfoMap = animation->GetIDMap();
            //         for (auto &node : boneInfoMap) {
            //             const int index = node.second.id;
            //             auto bone = animation->FindBone(node.first);
            //             if (!bone)
            //                 continue;

            //             if (ImGui::TreeNode(node.first.c_str())) {
            //                 bone->SetLocalTransform();
            //                 Editor::GUI::DrawVec3Control("Position", bone->m_pos);
            //                 bone->m_rot = glm::degrees(bone->m_rot);
            //                 Editor::GUI::DrawVec3Control("Rotation", bone->m_rot);
            //                 bone->m_rot = glm::radians(bone->m_rot);
            //                 Editor::GUI::DrawVec3Control("Scale", bone->m_scal, 1.0);
            //                 ImGui::TreePop();
            //             }
            //         }

            //         ImGui::TreePop();
            //     }
            // }
        });
    }

    void ModelRenderer::Draw(VaultRenderer::Shader &shader, const glm::mat4 &_model) {
        shader.SetUniform1i("playAnimation", play_animation);
        for (auto &mesh : model->meshes) {
            shader.SetUniformMat4("transformModel", _model * mesh.transform);
            mesh.Draw(shader);
        }
    }

    // void ModelRenderer::LoadMaterial(const std::string &path) {
    //     material_path = path;
    //     if (mesh)
    //         Serializer::DeserializeMaterial(path, mesh->material);
    // }

    void ModelRenderer::SetAnimation(const std::string &path) {
        if (!model)
            return;

        animation.reset();
        animator.reset();
        animation = std::make_unique<Animation>(path, Model::GlobalBoneMaps[model->path]);
        animator = std::make_unique<Animator>(animation.get());
    }

    void ModelRenderer::Init() {
        transform = &Scene::Main->EntityRegistry.get<Transform>(entity);
    }

    void ModelRenderer::AnimateAndSetUniforms(VaultRenderer::Shader &shader) {
        if (!animator)
            return;

        if (play_animation)
            animator->UpdateAnimation(Runtime::instance->timestep * time_scale);

        auto &transforms = animator->GetFinalBoneMatrices();
        shader.Bind();
        for (int i = 0; i < transforms.size(); ++i) {
            shader.SetUniformMat4(("finalBonesMatrices[" + std::to_string(i) + "]").c_str(), transforms[i]);
        }
    }

    void ModelRenderer::Animate() {
        animator->UpdateAnimation(Runtime::instance->timestep * time_scale);
    }
} // namespace Engine::Components