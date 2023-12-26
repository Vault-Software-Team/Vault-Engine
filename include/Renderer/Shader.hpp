#pragma once
#include <dllapi.hpp>
#include <cstdint>
#include <string>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#else
#include <glad/glad.h>
#endif
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

namespace VaultRenderer {
    class DLL_API Shader {
    private:
        static DLL_API Shader *binded_shader;
        uint32_t ID;

    public:
        Shader(const std::string &shader_file);
        std::string path;

        void Bind();

        void Unbind();

        // set uniforms
        void SetUniform1i(const char *name, int value);

        void SetUniform1f(const char *name, float value);

        void SetUniform2f(const char *name, float v0, float v1);

        void SetUniform3f(const char *name, float v0, float v1, float v2);

        void SetUniform4f(const char *name, float v0, float v1, float v2,
                          float v3);

        void SetUniformMat4(const char *name, glm::mat4 value);
        // uint
        void SetUniform1ui(const char *name, uint32_t value);
    };
} // namespace VaultRenderer