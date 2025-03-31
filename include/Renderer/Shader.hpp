#pragma once
#include <dllapi.hpp>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
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
        uint32_t OldID;

    public:
        enum ShaderType {
            NONE = -1,
            VERTEX = 0,
            FRAGMENT = 1,
            GEOMETRY = 2
        };

        static std::unordered_map<std::string, Shader *>
            shaders;

        Shader(const std::string &shader_file, bool CustomShader = false);
        ~Shader();
        const std::string path;
        const bool EngineShader;

        std::vector<ShaderType> types;
        std::vector<std::string> active_uni_names;
        std::vector<std::string> active_attrib_names;

        void Build(bool rebuild = false);
        void Delete();
        void Rebuild();

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