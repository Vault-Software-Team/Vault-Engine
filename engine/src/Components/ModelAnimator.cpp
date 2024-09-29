#include "Engine/Components/BoneManipulator.hpp"
#include "Engine/Components/MeshRenderer.hpp"
#include "Engine/GameObject.hpp"
#include "Engine/Model.hpp"
#include "Engine/Scene.hpp"
#include "Renderer/Shader.hpp"
#include "imgui/imgui.h"
#include <Engine/Components/ModelAnimator.hpp>
#include <Editor/GUI/MainGUI.hpp>
#include <icons/icons.h>
#include <Engine/SceneSerialization.hpp>
#include <Engine/Runtime.hpp>
#include <string>

namespace Engine::Components {
    void ModelAnimator::OnGUI() {
        DrawComponent<ModelAnimator>(Scene::Main->EntityRegistry, 40, [&] {
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

            ImGui::DragFloat("Animation Time Scale", &time_scale, 0.01);
            if (animation_path != "") {
                if (ImGui::Button(play_animation ? "Stop Animation" : "Play Animation")) {
                    play_animation = !play_animation;
                }
            }

            if (ImGui::TreeNode("Animations")) {
                static std::string GUI_AnimationName = "new_anim";
                ImGui::InputText("##ModelAnimatorAnimName", &GUI_AnimationName);
                if (ImGui::Button(ICON_FA_PLUS " Create New Animation")) {
                    if (animations.find(GUI_AnimationName) == animations.end() && GUI_AnimationName != "") {
                        animations[GUI_AnimationName] = nullptr;
                        GUI_AnimationName = "new_anim";
                    }
                }
                ImGui::NewLine();
                ImGui::Text("Current Animation");
                ImGui::InputText("##ModelAnimatorCurrAnim", &current_animation);

                for (auto &anim_data : animations) {
                    if (ImGui::TreeNode(anim_data.first.c_str())) {
                        ImGui::Button("Drag Animation/Model File");
                        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(2)) {
                            anim_data.second.reset();
                            animation_path = "";
                        }
                        if (ImGui::BeginDragDropTarget()) {
                            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("model_file")) {
                                std::string path = (char *)payload->Data;
                                animation_path = path;
                                SetAnimation(anim_data.first, animation_path);
                                play_animation = true;
                            }
                        }
                        if (anim_data.second.get()) {
                            ImGui::Text("%s", anim_data.second->animation_path.c_str());
                        }

                        ImGui::TreePop();
                    }
                }

                ImGui::TreePop();
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

    void ModelAnimator::BindToShader(VaultRenderer::Shader &shader) {
        shader.Bind();
        shader.SetUniform1i("playAnimation", play_animation);

        if (!animator)
            return;

        if (!play_animation) return;

        auto find_anim = animations.find(current_animation);
        if (find_anim == animations.end()) return;
        auto &animation = find_anim->second;

        auto &transforms = animator->GetFinalBoneMatrices();
        auto v = Scene::Main->EntityRegistry.view<BoneManipulator>();

        for (int i = 0; i < transforms.size(); ++i) {
            glm::mat4 bone_transform(1.0);

            for (auto &go : Scene::Main->GameObjects) {
                if (go->parent != ID) continue;
                if (!go->HasComponent<BoneManipulator>()) continue;

                auto &comp = go->GetComponent<BoneManipulator>();

                auto &boneInfoMap = animation->GetIDMap();
                if (boneInfoMap.find(comp.nodeName) != boneInfoMap.end()) {
                    int index = boneInfoMap[comp.nodeName].id;
                    if (index != i) continue;

                    auto &transform = go->GetComponent<Transform>();
                    bone_transform = transform.UpdateModelWithoutParent();
                }
            }

            shader.SetUniformMat4(("finalBonesMatrices[" + std::to_string(i) + "]").c_str(), bone_transform * transforms[i]);
        }
    }

    void ModelAnimator::SetAnimation(const std::string &animation_name, const std::string &path) {
        if (!model) return;
        animations[animation_name] = std::make_unique<Animation>(path, Model::GlobalBoneMaps[model->path]);
        if (animations.size() <= 1) animator = std::make_unique<Animator>(animations[animation_name].get());
        // animator.reset();

        // animation.reset();
        // animation = std::make_unique<Animation>(path, Model::GlobalBoneMaps[model->path]);
        // animator = std::make_unique<Animator>(animation.get());
    }

    void ModelAnimator::Init() {
        auto &gameObject = GameObject::FindGameObjectByEntity(entity);
        transform = &Scene::Main->EntityRegistry.get<Transform>(entity);

        std::string path = "";

        for (auto &child : Scene::Main->GameObjects) {
            if (child->parent != gameObject->ID) return;
            if (!child->HasComponent<MeshRenderer>()) return;

            auto &comp = child->GetComponent<MeshRenderer>();
            comp.animator_ref = this;
            path = comp.mesh_path;
        }

        model = path != "" ? std::make_unique<ModelMesh>(path) : nullptr;
    }

    void ModelAnimator::PlayAnimation() {
        if (!animator)
            return;

        auto find_anim = animations.find(current_animation);
        if (find_anim == animations.end()) return;
        auto &animation = find_anim->second;

        if (animator->curr_anim != animation.get())
            animator->SetCurrentAnimation(animation.get());

        if (play_animation)
            animator->UpdateAnimation(Runtime::instance->timestep * time_scale);

        // auto &transforms = animator->GetFinalBoneMatrices();
        // shader.Bind();
        // for (int i = 0; i < transforms.size(); ++i) {
        //     shader.SetUniformMat4(("finalBonesMatrices[" + std::to_string(i) + "]").c_str(), transforms[i]);
        // }
    }

    void ModelAnimator::AnimateAndSetUniforms(VaultRenderer::Shader &shader) {
        if (!animator)
            return;

        auto find_anim = animations.find(current_animation);
        if (find_anim == animations.end()) return;
        auto &animation = find_anim->second;

        if (animator->curr_anim != animation.get())
            animator->SetCurrentAnimation(animation.get());

        if (play_animation)
            animator->UpdateAnimation(Runtime::instance->timestep * time_scale);

        auto &transforms = animator->GetFinalBoneMatrices();
        shader.Bind();
        for (int i = 0; i < transforms.size(); ++i) {
            shader.SetUniformMat4(("finalBonesMatrices[" + std::to_string(i) + "]").c_str(), transforms[i]);
        }
    }

    void ModelAnimator::Animate() {
        animator->UpdateAnimation(Runtime::instance->timestep * time_scale);
    }
} // namespace Engine::Components