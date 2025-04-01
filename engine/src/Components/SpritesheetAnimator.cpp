#include "Engine/Components/SpritesheetRenderer.hpp"
#include "Engine/Runtime.hpp"
#include "imgui/imgui.h"
#include <Engine/Components/SpritesheetAnimator.hpp>
#include <Engine/Scene.hpp>
#include <iostream>
#include <Editor/GUI/MainGUI.hpp>
#include <icons/icons.h>
#include <imgui/imgui_stdlib.h>

using namespace VaultRenderer;
namespace Engine::Components {
    void SpritesheetAnimator::Init() {
        transform = &Scene::Main->EntityRegistry.get<Transform>(entity);
        animations["default_animation"] = Animation{
            .frames = std::vector<Frame>({Frame{}})
            //
        };

        float xCoord = animations["default_animation"].frames[0].spriteOffset.x + animations["default_animation"].frames[0].spriteSize.x;
        float yCoord = (spritesheetSize.y - (animations["default_animation"].frames[0].spriteOffset.y + animations["default_animation"].frames[0].spriteSize.y)) + animations["default_animation"].frames[0].spriteSize.y;
        std::vector<glm::vec2> texCoords = {
            glm::vec2(animations["default_animation"].frames[0].spriteOffset.x / spritesheetSize.x, (spritesheetSize.y - (animations["default_animation"].frames[0].spriteOffset.y + animations["default_animation"].frames[0].spriteSize.y)) / spritesheetSize.y),
            glm::vec2(xCoord / spritesheetSize.x, (spritesheetSize.y - (animations["default_animation"].frames[0].spriteOffset.y + animations["default_animation"].frames[0].spriteSize.y)) / spritesheetSize.y),
            glm::vec2(xCoord / spritesheetSize.x, yCoord / spritesheetSize.y),
            glm::vec2(animations["default_animation"].frames[0].spriteOffset.x / spritesheetSize.x, yCoord / spritesheetSize.y) //
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
        mesh->material.roughness = 1;
        mesh->material.metallic = 0;
    }

    void SpritesheetAnimator::Draw(VaultRenderer::Shader &shader) {

        if (mesh)
            mesh->Draw(shader);
    }

    void SpritesheetAnimator::OnGUI() {
        DrawComponent<SpritesheetAnimator>(Scene::Main->EntityRegistry, 40, [&] {
            ImGui::ColorEdit4("Color", &mesh->material.color.x);

            ImGui::Text("Sprite");
            ImGui::ImageButton((mesh->material.diffuse ? reinterpret_cast<void *>(mesh->material.diffuse->texture_data->ID) : 0), ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
            if (ImGui::IsItemClicked(1)) {
                mesh->material.diffuse.reset();
            }
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("image_file")) {
                    std::string path = (char *)payload->Data;
                    mesh->material.SetDiffuse(path, true);
                }
            }

            Editor::GUI::DrawVec2Control("Sheet Size", spritesheetSize);
            ImGui::InputText("Current Animation", &current_animation);
            ImGui::DragFloat("Time Between Frames", &time_between_frames, 0.01);
            ImGui::Checkbox("Animate", &animate);
            ImGui::Checkbox("Test Animation", &animate_without_runtime);

            ImGui::Button("Drag XML Animation File");
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("xml_file")) {
                    std::string path = (char *)payload->Data;
                    animations = GetAnimationsFromXML(time_between_frames, spritesheetSize, path);
                }
            }

            static std::string animation_name = "New Animation";
            if (ImGui::TreeNode("Animations")) {
                ImGui::InputText("Name", &animation_name);
                if (ImGui::Button("Add Animation")) {
                    animations[animation_name].frames = std::vector<Frame>({
                        Frame{
                            .spriteSize = {512, 512},
                            .spriteOffset = {0, 0},
                            //
                        }
                        //
                    });
                }

                for (auto &animation : animations) {
                    if (ImGui::TreeNode((std::string(ICON_FA_CUBE) + " " + animation.first).c_str())) {
                        if (ImGui::Button("Add Frame")) {
                            animation.second.frames.push_back(Frame{
                                .spriteSize = {512, 512},
                                .spriteOffset = {0, 0},
                                //
                            });
                        }
                        ImGui::Checkbox("Loop", &animation.second.loop);

                        for (int i = 0; i < animation.second.frames.size(); i++) {
                            Frame &frame = animation.second.frames[i];
                            if (ImGui::TreeNode(("Frame " + std::to_string(i)).c_str())) {
                                Editor::GUI::DrawVec2Control("Sprite Size", frame.spriteSize);
                                Editor::GUI::DrawVec2Control("Sprite Offset", frame.spriteOffset);

                                if (ImGui::Button(ICON_FA_TRASH " Remove Frame")) animation.second.frames.erase(animation.second.frames.begin() + i);

                                ImGui::TreePop();
                            }
                        }

                        ImGui::TreePop();
                    }
                }

                ImGui::TreePop();
            }

            ImGui::NewLine();
        });
    }

    void SpritesheetAnimator::Start() {
        time_left = time_between_frames;
    }

    void SpritesheetAnimator::SetAnimation(const std::string &animation_name) {
        current_frame_index = 0;
        time_left = time_between_frames;
        current_animation = animation_name;
    }
    void SpritesheetAnimator::Update() {
        if (!animate_without_runtime) {
            current_frame_index = 0;
            time_left = time_between_frames;
        }
        if (mesh == nullptr || !animate || (!Runtime::instance->isRunning && !animate_without_runtime))
            return;

        auto find_anim = animations.find(current_animation);
        if (find_anim == animations.end()) return;
        auto &animation = find_anim->second;
        time_left -= Runtime::instance->timestep;

        if (time_left <= 0) {
            current_frame_index++;
            if (current_frame_index >= animation.frames.size()) {
                if (animation.loop)
                    current_frame_index = 0;
                else
                    return;
            }

            for (auto &vertex : mesh->vertices) {
                int index = &vertex - &mesh->vertices[0];
                float xCoord = animation.frames[current_frame_index].spriteOffset.x + animation.frames[current_frame_index].spriteSize.x;
                float yCoord =
                    (spritesheetSize.y - (animation.frames[current_frame_index].spriteOffset.y + animation.frames[current_frame_index].spriteSize.y)) +
                    animation.frames[current_frame_index].spriteSize.y;

                std::vector<glm::vec2> texCoords = {
                    glm::vec2(
                        animation.frames[current_frame_index].spriteOffset.x / spritesheetSize.x,
                        (spritesheetSize.y - (animation.frames[current_frame_index].spriteOffset.y + animation.frames[current_frame_index].spriteSize.y)) /
                            spritesheetSize.y),
                    glm::vec2(
                        xCoord / spritesheetSize.x,
                        (spritesheetSize.y - (animation.frames[current_frame_index].spriteOffset.y + animation.frames[current_frame_index].spriteSize.y)) /
                            spritesheetSize.y),
                    glm::vec2(xCoord / spritesheetSize.x,
                              yCoord / spritesheetSize.y),
                    glm::vec2(animation.frames[current_frame_index].spriteOffset.x / spritesheetSize.x,
                              yCoord / spritesheetSize.y)};

                vertex.texUV = texCoords[index];
            }

            glBindVertexArray(mesh->VAO);
            glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * mesh->vertices.size(), mesh->vertices.data());
            time_left = time_between_frames;
        }
    }

    std::unordered_map<std::string, SpritesheetAnimator::Animation> SpritesheetAnimator::GetAnimationsFromXML(float delay, glm::vec2 sheetSize, const std::string &xmlFile) {
        tinyxml2::XMLDocument doc;
        doc.LoadFile(xmlFile.c_str());

        tinyxml2::XMLElement *root = doc.FirstChildElement("TextureAtlas");
        tinyxml2::XMLElement *sub_texture = root->FirstChildElement("SubTexture");

        std::string current_animation = "";
        std::unordered_map<std::string, SpritesheetAnimator::Animation> animations;
        std::vector<SpritesheetAnimator::Frame> frames;

        int frame_index = 0;

        while (sub_texture) {
            std::string name = sub_texture->Attribute("name");
            std::string x = sub_texture->Attribute("x");
            std::string y = sub_texture->Attribute("y");
            std::string width = sub_texture->Attribute("width");
            std::string height = sub_texture->Attribute("height");

            // remove last 4 digits???
            std::string animation_name = name.substr(0, name.size() - 4);
            if (current_animation == "") current_animation = animation_name;

            std::cout << animation_name << " | " << current_animation << frames.size() << "\n";

            if (current_animation != animation_name) {
                // std::cout << name << " | " << current_animation << frames.size() << "\n";
                animations[current_animation].frames = frames;
                current_animation = animation_name;
                frames.clear();
                frame_index = 0;
            }

            Frame frame;
            frame.spriteSize = glm::vec2(std::stof(width), std::stof(height));
            frame.spriteOffset = glm::vec2(std::stof(x), std::stof(y));
            frames.push_back(frame);

            sub_texture = sub_texture->NextSiblingElement("SubTexture");
        }

        doc.Clear();

        return animations;
    }
} // namespace Engine::Components