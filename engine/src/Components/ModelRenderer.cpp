#include <Engine/Components/ModelRenderer.hpp>
#include <Editor/GUI/MainGUI.hpp>
#include <icons/icons.h>
#include <Engine/SceneSerialization.hpp>
#include <Engine/Runtime.hpp>

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
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("model_file")) {
                    std::string path = (char *)payload->Data;
                    animation_path = path;
                    SetAnimation(animation_path);
                }
            }
            ImGui::Text("%s", animation_path.c_str());
            ImGui::DragFloat("Animation Time Scale", &time_scale, 0.01);
        });
    }

    void ModelRenderer::Draw(VaultRenderer::Shader &shader) {
        for (auto &mesh : model->meshes) {
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

    void ModelRenderer::AnimateAndSetUniforms(VaultRenderer::Shader &shader) {
        if (!animator)
            return;

        animator->UpdateAnimation(Runtime::instance->timestep * time_scale);

        auto transforms = animator->GetFinalBoneMatrices();
        shader.Bind();
        for (int i = 0; i < transforms.size(); ++i) {
            shader.SetUniformMat4(("finalBonesMatrices[" + std::to_string(i) + "]").c_str(), transforms[i]);
        }
    }
} // namespace Engine::Components