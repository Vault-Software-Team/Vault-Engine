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
        struct DLL_API ModelAnimator : Base {
            static inline const std::string display_name = "Model Animator";
            std::unique_ptr<ModelMesh> model;
            std::unique_ptr<Animation> animation;
            std::unique_ptr<Animator> animator;
            Transform *transform = nullptr;
            std::string animation_path;

            float time_scale = 1.0;
            bool play_animation = false;

            ModelAnimator() = default;

            void OnGUI() override;
            void Init() override;
            void SetAnimation(const std::string &path);
            void BindToShader(VaultRenderer::Shader &shader);
            void Animate();
            void PlayAnimation();
            void AnimateAndSetUniforms(VaultRenderer::Shader &shader);
        };
    } // namespace Components
} // namespace Engine