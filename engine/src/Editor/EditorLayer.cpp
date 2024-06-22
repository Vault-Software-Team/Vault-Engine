#include "Renderer/Shader.hpp"
#include "glm/gtx/quaternion.hpp"
#include "imgui/imgui.h"
#include <imgui/ImGuiNotify.hpp>
#include <Editor/GUI/MainGUI.hpp>
#include <Editor/EditorLayer.hpp>
#include <Renderer/Stats.hpp>
#include <ImGuiFileDialog/ImGuiFileDialog.h>
#include <icons/icons.h>
#include <memory>

namespace Editor {
    // Variable Definition
    DLL_API EditorLayer *EditorLayer::instance;

    EditorLayer::EditorLayer() {
        using namespace VaultRenderer;
        instance = this;

        ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeFile, "", ImVec4(1, 1, 1, 1.0f), ICON_FA_FILE);
        ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeDir, "", ImVec4(1, 1, 1, 1.0f), ICON_FA_FOLDER);

        IconShader = std::make_unique<Shader>("./shaders/editor_icons.glsl");

        std::vector<Vertex> _v = {
            //
            Vertex{glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0, 1, 0)},
            Vertex{glm::vec3(0.5f, -0.5f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec3(0, 1, 0)},
            Vertex{glm::vec3(0.5f, 0.5f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec3(0, 1, 0)},
            Vertex{glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec3(0, 1, 0)}
            //
        };
        std::vector<uint32_t> _i = {0, 1, 2, 0, 2, 3};
        IconMesh = std::make_unique<Mesh>(_v, _i);

        iconMeshes.CameraIcon = std::make_unique<Mesh>(_v, _i);
        iconMeshes.PointLightIcon = std::make_unique<Mesh>(_v, _i);
        iconMeshes.DirLightIcon = std::make_unique<Mesh>(_v, _i);
        iconMeshes.SpotLightIcon = std::make_unique<Mesh>(_v, _i);
        iconMeshes.AudioIcon = std::make_unique<Mesh>(_v, _i);

        icons.CameraIcon = std::make_unique<Texture>("./editor/icons/camera.png", TEXTURE_DIFFUSE);
        icons.PointLightIcon = std::make_unique<Texture>("./editor/icons/point_light.png", TEXTURE_DIFFUSE);
        icons.DirLightIcon = std::make_unique<Texture>("./editor/icons/directional_light.png", TEXTURE_DIFFUSE);
        icons.SpotLightIcon = std::make_unique<Texture>("./editor/icons/spot_light.png", TEXTURE_DIFFUSE);
        icons.AudioIcon = std::make_unique<Texture>("./editor/icons/audio_icon.png", TEXTURE_DIFFUSE);
    }

    void EditorLayer::GUI() {
        GUI::MenuBar();

        GUI::Hierarchy();
        GUI::Assets();
        GUI::Console();
        GUI::Scene();
        GUI::Components();
        GUI::MaterialInspector();
        GUI::BottomLogCounter();
        GUI::Config();
        GUI::DeveloperConsole();

        ImGui::Begin("Statistics");
        ImGui::Text("Draw Calls: %d", VaultRenderer::Statistics::GetDrawCalls());
        ImGui::Text("FPS: %d", VaultRenderer::Statistics::fps);
        ImGui::Text("Vendor: %s", VaultRenderer::Statistics::vendor.c_str());
        ImGui::Text("Renderer: %s", VaultRenderer::Statistics::renderer.c_str());
        ImGui::Text("Version: %s", VaultRenderer::Statistics::version.c_str());
        ImGui::Text("Shading Language: %s", VaultRenderer::Statistics::shading_language.c_str());
        ImGui::End();

        ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);   // Disable round borders
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f); // Disable borders

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 1.00f)); // Background color

        ImGui::RenderNotifications();

        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(1);
    }

    void EditorLayer::GameGUI() {
        GUI::Game_Scene();
    }

    void EditorLayer::DrawIcon(std::unique_ptr<VaultRenderer::Mesh> &icon_mesh, VaultRenderer::Shader &shader, const glm::mat4 &transform, uint32_t entity, std::unique_ptr<VaultRenderer::Texture> &texture) {
        shader.Bind();
        shader.SetUniform1ui("u_EntityID", entity);
        shader.SetUniformMat4("transformModel", transform);
        shader.SetUniform4f("baseColor", 1, 1, 1, 1);

        texture->Bind(0);
        shader.SetUniform1i("texture_diffuse.tex", 0);
        shader.SetUniform1i("texture_diffuse.defined", 1);

        icon_mesh->Draw(shader, false);

        shader.Unbind();
    }

    void EditorLayer::s_ColliderGizmo::DrawGizmo(VaultRenderer::Shader &shader, VaultRenderer::Mesh *mesh, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 colliderScale) {
        shader.Bind();
        glm::mat4 model = glm::translate(glm::mat4(1.0f), position) *
                          glm::toMat4(glm::quat(rotation)) *
                          glm::scale(glm::mat4(1.0f), colliderScale * glm::vec3(0.5, 0.5, 0.5));

        shader.SetUniformMat4("transformModel", model);
        mesh->Draw(shader, false);
    }

} // namespace Editor