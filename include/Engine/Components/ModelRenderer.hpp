#pragma once
#include <dllapi.hpp>
#include <glm/ext.hpp>
#include "Base.hpp"
#include <Renderer/Mesh.hpp>
#include <Renderer/Shader.hpp>
#include <memory>
#include <Renderer/Texture.hpp>
#include <Engine/Model.hpp>

namespace Engine {
    namespace Components {
        struct DLL_API ModelRenderer : Base {
            static inline const std::string display_name = "Model Renderer";
            std::unique_ptr<ModelMesh> model;
            // Model Animation
            std::unique_ptr<Animation> animation;
            std::unique_ptr<Animator> animator;
            std::string animation_path;
            float time_scale = 1.0;

            ModelRenderer() = default;

            void OnGUI() override;
            void Draw(VaultRenderer::Shader &shader);
            void SetAnimation(const std::string &path);
            void AnimateAndSetUniforms(VaultRenderer::Shader &shader);
        };
    } // namespace Components
} // namespace Engine