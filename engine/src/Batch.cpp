#include <Engine/Batch.hpp>
#include <algorithm>
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
#include <Engine/Components/MeshRenderer.hpp>
#include <Engine/Components/Transform.hpp>
#include <Engine/Scene.hpp>

using namespace VaultRenderer;
namespace Engine {
    Batch::Batch(entt::entity entity, std::unique_ptr<Mesh> &mesh) {
        // VAO setup
        glGenVertexArrays(1, &VAO);

        for (auto &vertex : mesh->vertices) {
            vertex.EntityID = (uint32_t)entity;
            // vertex.modelIndex = mesh_max_index;
        }
        entities.push_back(entity);
        vertices.insert(vertices.end(), mesh->vertices.begin(), mesh->vertices.end());

        uint32_t max_index = -1;

        for (uint32_t &index : indices) {
            if (index != 0 && index > max_index) {
                max_index = index;
            }
        }

        for (uint32_t &index : mesh->indices) {
            uint32_t offsetted_index = index + (max_index + 1);

            indices.push_back(offsetted_index);
        }

        // VBO Setup
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_DYNAMIC_DRAW);

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

        glVertexAttribPointer(5, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, EntityID));
        glEnableVertexAttribArray(5);

        // glVertexAttribPointer(6, 1, GL_INT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, modelIndex));
        // glEnableVertexAttribArray(6);

        // EBO Setup

        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glBindVertexArray(mesh->VAO);
        // glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
        // glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * mesh->vertices.size(),
        // mesh->vertices.data());
        mesh_max_index++;
    }

    void Batch::AddMesh(entt::entity entity, std::unique_ptr<VaultRenderer::Mesh> &mesh) {
        for (auto &vertex : mesh->vertices) {
            vertex.EntityID = (uint32_t)entity;
            // vertex.modelIndex = mesh_max_index;
        }
        entities.push_back(entity);
        vertices.insert(vertices.end(), mesh->vertices.begin(), mesh->vertices.end());

        uint32_t max_index = 0;

        for (uint32_t &index : indices) {
            if (index != 0 && index > max_index) {
                max_index = index;
            }
        }

        for (uint32_t &index : mesh->indices) {
            uint32_t offsetted_index = index + (max_index == 0 ? max_index : max_index + 1);
            std::cout << max_index << " " << offsetted_index << "\n";

            indices.push_back(offsetted_index);
        }
        // for (uint32_t &index : indices) {
        // std::cout << index << "\n";
        // }

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * vertices.size(), vertices.data());
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(uint32_t) * indices.size(), indices.data());

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        mesh_max_index++;
    }

    void Batch::Draw(Shader &shader) {
        using namespace Components;
        Statistics::DrawCall();

        for (int i = 0; i < entities.size(); i++) {
            auto &transform = Scene::Main->EntityRegistry.get<Transform>(entities[i]);
            shader.SetUniformMat4(("transforms[" + std::to_string(i) + "]").c_str(), transform.model);
        }

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * vertices.size(), vertices.data());
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(uint32_t) * indices.size(), indices.data());
        glDrawElements(GL_TRIANGLES, static_cast<uint32_t>(indices.size()), GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
    }
} // namespace Engine