#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

namespace SimpleCalls {
    extern uint32_t cubeVAO;
    extern uint32_t cubeVBO;
    void RenderCube();

    extern uint32_t quadVAO;
    extern uint32_t quadVBO;
    void RenderQuad();
} // namespace SimpleCalls