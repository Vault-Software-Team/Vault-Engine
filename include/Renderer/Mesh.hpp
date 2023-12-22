#pragma once
#include <glm/ext.hpp>
#include <vector>
#include <Renderer/Shader.hpp>
#include <dllapi.hpp>

namespace VaultRenderer {
    struct DLL_API Vertex {
        glm::vec3 position;
        glm::vec2 texUV = glm::vec2(0, 0);
    };

    class DLL_API Mesh {
    private:
        uint32_t VBO, VAO, EBO;

    public:
        Mesh(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices);

        void Draw(Shader &shader);

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
    };
} // namespace VaultRenderer