#include <Engine/Components/SpritesheetRenderer.hpp>
#include <Engine/Scene.hpp>
#include <iostream>
#include <Editor/GUI/MainGUI.hpp>
#include <icons/icons.h>
#include <imgui/imgui_stdlib.h>

using namespace VaultRenderer;
namespace Engine::Components {
    void SpritesheetRenderer::Init() {
        transform = &Scene::Main->EntityRegistry.get<Transform>(entity);

        float xCoord = spriteOffset.x + spriteSize.x;
        float yCoord = (spritesheetSize.y - (spriteOffset.y + spriteSize.y)) + spriteSize.y;
        std::vector<glm::vec2> texCoords = {
            glm::vec2(spriteOffset.x / spritesheetSize.x, (spritesheetSize.y - (spriteOffset.y + spriteSize.y)) / spritesheetSize.y),
            glm::vec2(xCoord / spritesheetSize.x, (spritesheetSize.y - (spriteOffset.y + spriteSize.y)) / spritesheetSize.y),
            glm::vec2(xCoord / spritesheetSize.x, yCoord / spritesheetSize.y),
            glm::vec2(spriteOffset.x / spritesheetSize.x, yCoord / spritesheetSize.y) //
        };

        std::vector<Vertex> vertices = {
            Vertex{glm::vec3(-0.5f, -0.5f, 0.0f), texCoords[0], glm::vec3(0, 1, 0)},

            Vertex{glm::vec3(0.5f, -0.5f, 0.0f), texCoords[1], glm::vec3(0, 1, 0)},

            Vertex{glm::vec3(0.5f, 0.5f, 0.0f), texCoords[2], glm::vec3(0, 1, 0)},

            Vertex{glm::vec3(-0.5f, 0.5f, 0.0f), texCoords[3], glm::vec3(0, 1, 0)}
            //
        };

        std::vector<uint32_t> indices = {0, 1, 2, 0, 2, 3};

        mesh = std::make_unique<Mesh>(vertices, indices);
    }

    void SpritesheetRenderer::Draw(VaultRenderer::Shader &shader) {
        if (mesh)
            mesh->Draw(shader);
    }

    void SpritesheetRenderer::OnGUI() {
        DrawComponent<SpritesheetRenderer>(Scene::Main->EntityRegistry, 40, [&] {
            ImGui::ColorEdit4("Color", &mesh->material.color.x);

            ImGui::Text("Sprite");
            ImGui::ImageButton((mesh->material.diffuse ? reinterpret_cast<void *>(mesh->material.diffuse->texture_data->ID) : 0), ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
            if (ImGui::IsItemClicked(1)) {
                mesh->material.diffuse.reset();
            }
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("image_file")) {
                    std::string path = (char *)payload->Data;
                    mesh->material.SetDiffuse(path);
                }
            }

            // ImGui::Button("Drag Custom Shader");
            // if (custom_shader_path != "") ImGui::Text("Custom Shader: %s", custom_shader_path.c_str());
            // if (ImGui::BeginDragDropTarget()) {
            //     if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("shader")) {
            //         custom_shader_path = (char *)payload->Data;
            //         custom_shader = std::make_unique<VaultRenderer::Shader>(custom_shader_path);
            //     }
            // }

            Editor::GUI::DrawVec2Control("Sheet Size", spritesheetSize);
            Editor::GUI::DrawVec2Control("Sprite Size", spriteSize);
            Editor::GUI::DrawVec2Control("Sprite Offset", spriteOffset);

            ImGui::NewLine();
        });
    }

    void SpritesheetRenderer::Update() {
        if (mesh == nullptr)
            return;

        bool trulyChanged = false;
        for (auto &vertex : mesh->vertices) {
            int index = &vertex - &mesh->vertices[0];
            float xCoord = spriteOffset.x + spriteSize.x;
            float yCoord =
                (spritesheetSize.y - (spriteOffset.y + spriteSize.y)) +
                spriteSize.y;

            std::vector<glm::vec2> texCoords = {
                glm::vec2(
                    spriteOffset.x / spritesheetSize.x,
                    (spritesheetSize.y - (spriteOffset.y + spriteSize.y)) /
                        spritesheetSize.y),
                glm::vec2(
                    xCoord / spritesheetSize.x,
                    (spritesheetSize.y - (spriteOffset.y + spriteSize.y)) /
                        spritesheetSize.y),
                glm::vec2(xCoord / spritesheetSize.x,
                          yCoord / spritesheetSize.y),
                glm::vec2(spriteOffset.x / spritesheetSize.x,
                          yCoord / spritesheetSize.y)};

            trulyChanged = !(vertex.texUV.x == texCoords[index].x && vertex.texUV.y == texCoords[index].y);

            vertex.texUV = texCoords[index];
        }

        if (trulyChanged) {
            glBindVertexArray(mesh->VAO);
            glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * mesh->vertices.size(), mesh->vertices.data());
        }
    }
} // namespace Engine::Components