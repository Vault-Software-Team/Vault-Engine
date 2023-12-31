#pragma once
#include <dllapi.hpp>
#include <Engine/Scene.hpp>
#include <Engine/GameObject.hpp>
#include <Renderer/Framebuffer.hpp>
#include <Renderer/Window.hpp>
#include <Renderer/ShadowMap.hpp>

namespace Engine {
    class DLL_API Runtime {
    private:
        bool paused = false;

    public:
        int width, height;
        static DLL_API Runtime *instance;
        static DLL_API VaultRenderer::Shader *default_shader;
        VaultRenderer::ShadowMap *shadowMap;

        bool isRunning = false;
        bool isStopped = true;
        float timestep = 0;

        Runtime(VaultRenderer::Shader *default_shader);

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
    };
} // namespace Engine