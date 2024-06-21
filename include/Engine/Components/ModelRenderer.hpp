#pragma once
#include <dllapi.hpp>
#include <glm/ext.hpp>
#include "Base.hpp"
#include <Renderer/Mesh.hpp>
#include <Renderer/Shader.hpp>
#include <memory>
#include <Renderer/Texture.hpp>
#include <Engine/Model.hpp>
#include <thread>

namespace Engine {
    namespace Components {
        struct DLL_API ModelRenderer : Base {
            static inline const std::string display_name = "Model Renderer";
            std::unique_ptr<ModelMesh> model;
            // Model Animation
            std::unique_ptr<Animation> animation;
            std::unique_ptr<Animator> animator;
            Transform *transform;
            std::string animation_path;
            glm::mat4 assimp_transform;
            float time_scale = 1.0;
            bool play_animation = false;

            ModelRenderer() = default;

            void OnGUI() override;
            void Init() override;
            void Draw(VaultRenderer::Shader &shader, const glm::mat4 &_model);
            void SetAnimation(const std::string &path);
            void AnimateAndSetUniforms(VaultRenderer::Shader &shader);
            void Animate();
        };
    } // namespace Components
} // namespace Engine