#pragma once
#include "Engine/CascadedShadowMap.hpp"
#include "Engine/Physics/BulletPhysics.hpp"
#include <dllapi.hpp>
#include <Engine/Scene.hpp>
#include <Engine/GameObject.hpp>
#include <Renderer/Framebuffer.hpp>
#include <Renderer/Window.hpp>
#include <Renderer/ShadowMap.hpp>
#include <functional>
#include <memory>
#include <Engine/HDRSkybox.hpp>
#include <thread>

namespace Engine {
    class DLL_API Runtime {
    private:
        bool paused = false;

    public:
        int width, height;
        static DLL_API void AspectRatioCameraViewport();
        static DLL_API Runtime *instance;
        static DLL_API VaultRenderer::Shader *default_shader;
        VaultRenderer::ShadowMap *shadowMap;
        VaultRenderer::HDRSkybox *HDR_Skybox;
        CascadedShadowMap *c_ShadowMap;
        std::unique_ptr<Physics3D> bulletPhysics;
        static bool usePBR;

        std::vector<std::function<void()>> main_thread_calls;

        bool isRunning = false;
        bool isStopped = true;
        float timestep = 0;

        Runtime(VaultRenderer::Shader *default_shader);
        ~Runtime();

        void SetDefaultShader(VaultRenderer::Shader *default_shader);
        void UpdateGameObjects(VaultRenderer::Window &window);
        void Pause();
        void Unpause();
        void SetGlobalUniforms();
        void DrawToShadowMap(VaultRenderer::DepthFramebuffer &shadowMap, VaultRenderer::Shader &shader);
        void UpdateMainCamera(VaultRenderer::Window &window);
        void ShadowShenanigans(VaultRenderer::ShadowMap &shadow_map);

        void BindShadowsToShader(VaultRenderer::Shader &shader, VaultRenderer::ShadowMap &shadow_map);
        bool Paused() const;

        void Scheduling_DeleteGameObjects();
        void Scheduling_SceneChange();

        void MouseEvents(entt::entity hoveredEntity);

        std::vector<glm::vec4> GetFrustumCornersWorldSpace(const glm::mat4 &proj, const glm::mat4 &view) {
            const auto inv = glm::inverse(proj * view);

            std::vector<glm::vec4> frustumCorners;
            for (uint32_t x = 0; x < 2; ++x) {
                for (uint32_t y = 0; y < 2; ++y) {
                    for (uint32_t z = 0; z < 2; ++z) {
                        const glm::vec4 pt = inv * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
                        frustumCorners.push_back(pt / pt.w);
                    }
                }
            }

            return frustumCorners;
        }
    };
} // namespace Engine