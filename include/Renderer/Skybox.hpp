#pragma once
#include <cstdint>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Renderer/Shader.hpp>
#include <glm/ext.hpp>

namespace VaultRenderer {
    class Skybox {
    public:
        uint32_t skyboxVAO, skyboxVBO, skyboxEBO, cubemapTexture;

        Skybox();
        void Render(Shader &shader, glm::vec3 camPos, glm::vec3 camRot, glm::vec3 camUp);
    };
} // namespace VaultRenderer