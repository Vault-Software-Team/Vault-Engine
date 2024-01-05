#pragma once
#include <dllapi.hpp>
#include <glm/ext.hpp>
#include <vector>
#include <Renderer/Shader.hpp>
#include <memory>
#include "Texture.hpp"

#define MAX_BONE_INFLUENCE 4
namespace VaultRenderer {
    class DLL_API Material {
    public:
        glm::vec4 color;
        std::unique_ptr<VaultRenderer::Texture> diffuse{nullptr};
        std::unique_ptr<VaultRenderer::Texture> specular{nullptr};
        std::unique_ptr<VaultRenderer::Texture> normal{nullptr};
        std::unique_ptr<VaultRenderer::Texture> height{nullptr};

        Material(const glm::vec4 &color = glm::vec4(1, 1, 1, 1));

        void SetDiffuse(const std::string &texture_path);
        void SetSpecular(const std::string &texture_path);
        void SetNormal(const std::string &texture_path);
        void SetHeight(const std::string &texture_path);

        void BindToShader(Shader &shader);
    };
    struct DLL_API Vertex {
        glm::vec3 position;
        glm::vec2 texUV = glm::vec2(0, 0);
        glm::vec3 normal = glm::vec3(0, 1, 0);
        glm::vec3 tangent = glm::vec3(0, 0, 0);
        glm::vec3 bitangent = glm::vec3(0, 0, 0);
        uint32_t EntityID;
        // int modelIndex;
        int m_BoneIDs[MAX_BONE_INFLUENCE];
        float m_Weights[MAX_BONE_INFLUENCE];
    };
    struct BoneInfo {
        int id;
        glm::mat4 offset;
    };

    class DLL_API Mesh {
    private:
        uint32_t VBO, VAO, EBO;

    public:
        Mesh(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices);

        void Draw(Shader &shader);

        Material material;
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
    };
} // namespace VaultRenderer