#include <Renderer/Shader.hpp>
#include <stdio.h>
#include <fstream>
#include <iostream>

namespace VaultRenderer {
    DLL_API Shader *Shader::binded_shader;
    std::unordered_map<std::string, Shader *> Shader::shaders;

    Shader::~Shader() {
        shaders.erase(path);
    }

    Shader::Shader(const std::string &shader_file, bool CustomShader) : path(shader_file), EngineShader(!CustomShader) {
        Build();
        std::cout << "Added a new GLSL shader: " << shader_file << "\n";
        shaders[path] = this;
    }

    void Shader::Build() {
        ShaderType type = NONE;

        std::string vertCode, fragCode, geometryCode, line;

        printf("%s\n", (std::string("Loading shader: ") + path).c_str());
        std::ifstream fstream_shader_file(path);
        if (!fstream_shader_file.is_open()) {
            std::cout << "Failed to open shader file" << std::endl;
        }

        while (getline(fstream_shader_file, line)) {
            if (line == "#shader vertex") {
                type = ShaderType::VERTEX;
                types.push_back(type);
            } else if (line == "#shader fragment") {
                type = ShaderType::FRAGMENT;
                types.push_back(type);
            } else if (line == "#shader geometry") {
                type = ShaderType::GEOMETRY;
                types.push_back(type);
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

        GLint numActiveAttribs = 0;
        GLint numActiveUniforms = 0;
        glGetProgramiv(ID, GL_ACTIVE_ATTRIBUTES, &numActiveAttribs);
        glGetProgramiv(ID, GL_ACTIVE_UNIFORMS, &numActiveUniforms);

        std::vector<GLchar> nameData(256);
        for (int unif = 0; unif < numActiveUniforms; ++unif) {
            GLint arraySize = 0;
            GLenum type = 0;
            GLsizei actualLength = 0;
            glGetActiveUniform(ID, unif, nameData.size(), &actualLength, &arraySize, &type, &nameData[0]);
            std::string name((char *)&nameData[0], actualLength);
            active_uni_names.push_back(name);
        }

        GLint maxAttribNameLength = 0;
        glGetProgramiv(ID, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttribNameLength);

        std::vector<GLchar> attrib_nameData(maxAttribNameLength);
        for (int attrib = 0; attrib < numActiveAttribs; ++attrib) {
            GLint arraySize = 0;
            GLenum type = 0;
            GLsizei actualLength = 0;
            glGetActiveAttrib(ID, attrib, attrib_nameData.size(), &actualLength, &arraySize, &type, &attrib_nameData[0]);
            std::string name((char *)&attrib_nameData[0], actualLength - 1);
            active_attrib_names.push_back(name);
        }
    }

    void Shader::Delete() {
        glUseProgram(0);
        glDeleteProgram(ID);
    }

    void Shader::Rebuild() {
        Delete();
        Build();
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