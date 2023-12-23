#include <Renderer/Shader.hpp>
#include <stdio.h>
#include <fstream>
#include <iostream>

namespace VaultRenderer {
    Shader::Shader(const std::string &shader_file) : path(shader_file) {
        enum ShaderType {
            NONE = -1,
            VERTEX = 0,
            FRAGMENT = 1,
            GEOMETRY = 2
        } type;

        std::string vertCode, fragCode, geometryCode, line;

        printf("%s", (std::string("Loading shader: ") + shader_file).c_str());
        std::ifstream fstream_shader_file(shader_file);
        if (!fstream_shader_file.is_open()) {
            std::cout << "Failed to open shader file" << std::endl;
        }

        while (getline(fstream_shader_file, line)) {
            if (line == "#shader vertex") {
                type = ShaderType::VERTEX;
            } else if (line == "#shader fragment") {
                type = ShaderType::FRAGMENT;
            } else if (line == "#shader geometry") {
                type = ShaderType::GEOMETRY;
            } else {
                switch (type) {
                case ShaderType::VERTEX:
                    vertCode += line + "\n";
                    break;
                case ShaderType::FRAGMENT:
                    fragCode += line + "\n";
                    break;
                case ShaderType::GEOMETRY:
                    geometryCode += line + "\n";
                    break;
                default:
                    break;
                }
            }
        }

        const char *vertShaderCode = vertCode.c_str();
        const char *fragShaderCode = fragCode.c_str();
        const char *geometryShaderCode = geometryCode.c_str();
        uint32_t vertShader, fragShader, geometryShader;
        int success;
        char infoLog[512];

        vertShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertShader, 1, &vertShaderCode, NULL);
        glCompileShader(vertShader);
        glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertShader, 512, NULL, infoLog);
            printf("Failed to compile Vertex Shader");
            std::cout << infoLog << std::endl;
        }

        fragShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragShader, 1, &fragShaderCode, NULL);
        glCompileShader(fragShader);
        glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
            printf("Failed to compile Fragment Shader");
            std::cout << infoLog << std::endl;
        }

        if (type == ShaderType::GEOMETRY) {
            geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometryShader, 1, &geometryShaderCode, NULL);
            glCompileShader(geometryShader);
            glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(geometryShader, 512, NULL, infoLog);
                printf("Failed to compile Geometry Shader");
                std::cout << infoLog << std::endl;
            }
        }

        ID = glCreateProgram();
        glAttachShader(ID, vertShader);
        glAttachShader(ID, fragShader);
        if (type == 2) {
            glAttachShader(ID, geometryShader);
        }
        glLinkProgram(ID);
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(ID, 512, NULL, infoLog);
            printf("Failed to link program");
            std::cout << infoLog << std::endl;
        }

        glDeleteShader(vertShader);
        glDeleteShader(fragShader);
        if (type == ShaderType::GEOMETRY) {
            glDeleteShader(geometryShader);
        }
    }

    void Shader::Bind() {
        if (binded_shader != this)
            glUseProgram(ID);

        binded_shader = this;
    }

    void Shader::Unbind() { glUseProgram(0); }

    void Shader::SetUniform1f(const char *name, float value) {
        glUniform1f(glGetUniformLocation(ID, name), value);
    }

    void Shader::SetUniform1i(const char *name, int value) {
        glUniform1i(glGetUniformLocation(ID, name), value);
    }

    void Shader::SetUniform1ui(const char *name, uint32_t value) {
        glUniform1ui(glGetUniformLocation(ID, name), value);
    }

    void Shader::SetUniform2f(const char *name, float value1, float value2) {
        glUniform2f(glGetUniformLocation(ID, name), value1, value2);
    }

    void Shader::SetUniform3f(const char *name, float value1, float value2,
                              float value3) {
        glUniform3f(glGetUniformLocation(ID, name), value1, value2, value3);
    }

    void Shader::SetUniform4f(const char *name, float value1, float value2,
                              float value3, float value4) {
        glUniform4f(glGetUniformLocation(ID, name), value1, value2, value3,
                    value4);
    }

    void Shader::SetUniformMat4(const char *name, glm::mat4 value) {
        glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE,
                           glm::value_ptr(value));
    }
} // namespace VaultRenderer