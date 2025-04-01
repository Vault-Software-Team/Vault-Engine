#pragma once
#include <dllapi.hpp>
#include <entt/entt.hpp>
#include "Base.hpp"
#include <glm/ext.hpp>
#include <Renderer/Mesh.hpp>
#include <string>
#include <unordered_map>
#include "Transform.hpp"
#include "glm/fwd.hpp"
#include <tinyxml.hpp>
#include <vector>

namespace Engine {
    namespace Components {
        struct DLL_API SpritesheetAnimator : Base {
            static inline const std::string display_name = "Spritesheet Animator";
            Transform *transform;

            struct Frame {
                glm::vec2 spriteSize{32, 32};
                glm::vec2 spriteOffset{0, 0};
            };

            struct Animation {
                std::vector<Frame> frames;
                bool loop;
            };

            std::unique_ptr<VaultRenderer::Mesh> mesh;

            glm::vec2 spritesheetSize{512, 512};
            std::unordered_map<std::string, Animation> animations;
            float time_between_frames = 0.1f;
            std::string current_animation = "";
            bool animate = true;
            float time_left = 0.f;
            int current_frame_index = 0;
            bool animate_without_runtime = false;

            std::unique_ptr<VaultRenderer::Shader> custom_shader;
            std::string custom_shader_path;
            SpritesheetAnimator() = default;
            void Init() override;

            void Draw(VaultRenderer::Shader &shader);
            void OnGUI() override;
            void Update() override;
            void Start();
            void SetAnimation(const std::string &animation_name);
            static std::unordered_map<std::string, Animation> GetAnimationsFromXML(float delay, glm::vec2 sheetSize, const std::string &xmlFile);
        };
    } // namespace Components
} // namespace Engine