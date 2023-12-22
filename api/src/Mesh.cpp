#include <Renderer/Mesh.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace VaultRenderer {
    Mesh::Mesh(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices) : vertices(vertices), indices(indices) {
        // VAO setup
        glGenVertexArrays(1, &VAO);

        // VBO Setup
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

        // position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
        glEnableVertexAttribArray(0);
        // example to add more parameters
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texUV));
        glEnableVertexAttribArray(1);

        // EBO Setup

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    void Mesh::Draw(Shader &shader) {
        shader.Bind();

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<uint32_t>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
} // namespace VaultRenderer