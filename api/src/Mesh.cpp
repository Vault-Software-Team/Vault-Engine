#include <Renderer/Mesh.hpp>
#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
    #define GL_GLEXT_PROTOTYPES
    #define EGL_EGLEXT_PROTOTYPES
#else
    #include <glad/glad.h>
#endif
#include <GLFW/glfw3.h>
#include <Renderer/Stats.hpp>
#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

namespace VaultRenderer {
    Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, const std::string &name) : name(name), vertices(vertices), indices(indices), material{glm::vec4(1, 1, 1, 1)} {
        // VAO setup
        glGenVertexArrays(1, &VAO);

        for (int i = 0; i < indices.size(); i += 3) {
            Vertex &v1 = vertices[indices[i]];     // 1
            Vertex &v2 = vertices[indices[i + 1]]; // 3
            Vertex &v3 = vertices[indices[i + 2]]; // 4

            glm::vec3 edge1 = v2.position - v1.position;
            glm::vec3 edge2 = v3.position - v1.position;
            glm::vec2 deltaUV1 = v2.texUV - v1.texUV;
            glm::vec2 deltaUV2 = v3.texUV - v1.texUV;

            float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

            glm::vec3 tangent;
            tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
            tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
            tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

            glm::vec3 bitangent;
            bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
            bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
            bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

            v1.tangent = tangent;
            v2.tangent = tangent;
            v3.tangent = tangent;

            v1.bitangent = bitangent;
            v2.bitangent = bitangent;
            v3.bitangent = bitangent;
        }

        // VBO Setup
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

        // position

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
        glEnableVertexAttribArray(0);
        // example to add more parameters
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texUV));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));
        glEnableVertexAttribArray(2);

        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, tangent));
        glEnableVertexAttribArray(3);

        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, bitangent));
        glEnableVertexAttribArray(4);

        glEnableVertexAttribArray(5);
        glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void *)offsetof(Vertex, m_BoneIDs));

        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, m_Weights));

        // EBO Setup

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        material.diffuse = nullptr;
        material.specular = nullptr;
        material.normal = nullptr;
        material.height = nullptr;
    }

    void Mesh::Draw(Shader &shader, bool bindMatToShader) {
        if (bindMatToShader) material.BindToShader(shader);
        Statistics::DrawCall();

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<uint32_t>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    Material::Material(const glm::vec4 &color) : color(color) {}

    void Material::SetDiffuse(const std::string &texture_path) {
        diffuse = std::make_unique<Texture>(texture_path, TEXTURE_DIFFUSE);
    }
    void Material::SetSpecular(const std::string &texture_path) {
        specular = std::make_unique<Texture>(texture_path, TEXTURE_SPECULAR);
    }
    void Material::SetNormal(const std::string &texture_path) {
        normal = std::make_unique<Texture>(texture_path, TEXTURE_NORMAL);
    }
    void Material::SetHeight(const std::string &texture_path) {
        height = std::make_unique<Texture>(texture_path, TEXTURE_HEIGHT);
    }
    void Material::SetRoughness(const std::string &texture_path) {
        roughness_map = std::make_unique<Texture>(texture_path, TEXTURE_HEIGHT);
    }
    void Material::SetMetallic(const std::string &texture_path) {
        metallic_map = std::make_unique<Texture>(texture_path, TEXTURE_HEIGHT);
    }
    void Material::SetAO(const std::string &texture_path) {
        ao_map = std::make_unique<Texture>(texture_path, TEXTURE_HEIGHT);
    }

    void Material::BindToShader(Shader &shader) {
        shader.Bind();
        for (uint32_t i = 0; i < 4; i++) {
            // unbind all texture
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        // Bind PBR Shit
        shader.SetUniform1f("material.ao", ao);
        shader.SetUniform1f("material.metallic", metallic);
        shader.SetUniform1f("material.roughness", roughness);

        shader.SetUniform4f("baseColor", color.r, color.g, color.b, color.a);
        shader.SetUniform4f("emissionColor", emissionColor.r, emissionColor.g, emissionColor.b, 1);

        if (diffuse && fs::exists(diffuse ? diffuse->texture_data->texture_filepath : "")) {
            diffuse->Bind(0);
            shader.SetUniform1i("texture_diffuse.tex", 0);
            shader.SetUniform1i("texture_diffuse.defined", 1);
        } else {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);

            shader.SetUniform1i("texture_diffuse.tex", -1);
            shader.SetUniform1i("texture_diffuse.defined", 0);
        }

        if (specular && fs::exists(specular ? specular->texture_data->texture_filepath : "")) {
            specular->Bind(1);
            shader.SetUniform1i("texture_specular.tex", 1);
            shader.SetUniform1i("texture_specular.defined", 1);
        } else {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, 0);

            shader.SetUniform1i("texture_specular.tex", -1);
            shader.SetUniform1i("texture_specular.defined", 0);
        }

        if (normal && fs::exists(normal ? normal->texture_data->texture_filepath : "")) {
            normal->Bind(2);
            shader.SetUniform1i("texture_normal.tex", 2);
            shader.SetUniform1i("texture_normal.defined", 1);
        } else {
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, 0);

            shader.SetUniform1i("texture_normal.tex", -1);
            shader.SetUniform1i("texture_normal.defined", 0);
        }

        if (height && fs::exists(height ? height->texture_data->texture_filepath : "")) {
            height->Bind(3);
            shader.SetUniform1i("texture_height.tex", 3);
            shader.SetUniform1i("texture_height.defined", 1);
        } else {
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, 0);

            shader.SetUniform1i("texture_height.tex", -1);
            shader.SetUniform1i("texture_height.defined", 0);
        }

        if (metallic_map && fs::exists(metallic_map ? metallic_map->texture_data->texture_filepath : "")) {
            metallic_map->Bind(4);
            shader.SetUniform1i("texture_metallic.tex", 4);
            shader.SetUniform1i("texture_metallic.defined", 1);
        } else {
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, 0);

            shader.SetUniform1i("texture_metallic.tex", -1);
            shader.SetUniform1i("texture_metallic.defined", 0);
        }

        if (roughness_map && fs::exists(roughness_map ? roughness_map->texture_data->texture_filepath : "")) {
            roughness_map->Bind(5);
            shader.SetUniform1i("texture_roughness.tex", 5);
            shader.SetUniform1i("texture_roughness.defined", 1);
        } else {
            glActiveTexture(GL_TEXTURE5);
            glBindTexture(GL_TEXTURE_2D, 0);

            shader.SetUniform1i("texture_roughness.tex", -1);
            shader.SetUniform1i("texture_roughness.defined", 0);
        }

        if (ao_map && fs::exists(ao_map ? ao_map->texture_data->texture_filepath : "")) {
            ao_map->Bind(6);
            shader.SetUniform1i("texture_ao.tex", 6);
            shader.SetUniform1i("texture_ao.defined", 1);
        } else {
            glActiveTexture(GL_TEXTURE6);
            glBindTexture(GL_TEXTURE_2D, 0);

            shader.SetUniform1i("texture_ao.tex", -1);
            shader.SetUniform1i("texture_ao.defined", 0);
        }
    }
} // namespace VaultRenderer