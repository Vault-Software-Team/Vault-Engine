#pragma once

#include "Renderer/Shader.hpp"
namespace Engine {
    class PostProcessing {
    public:
        static bool GlobalBloom;
        static float BloomThreshold;
        static glm::vec3 BloomMultiplier;
        static bool PBR_IBL;

        static void BindToShader(VaultRenderer::Shader &shader, const std::string &u_StructName = "config_PostProcessing");
    };
} // namespace Engine