#include <Editor/GUI/MainGUI.hpp>
#include <Engine/Scene.hpp>
#include <Engine/GameObject.hpp>
#include <Engine/Components/IncludeComponents.hpp>
#include <icons/icons.h>
#include <imgui/imgui.h>
#include <Renderer/Window.hpp>
#include <ImGuizmo/ImGuizmo.h>

using namespace Engine;
using namespace Engine::Components;
using namespace VaultRenderer;

namespace Editor {
    void GUI::Scene() {
        static int m_GuizmoMode = ImGuizmo::OPERATION::TRANSLATE;
        static int m_GuizmoWorld = ImGuizmo::MODE::WORLD;
        static float bounds[] = {-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f};
        static float boundsSnap[] = {0.1f, 0.1f, 0.1f};

        ImGuiStyle &style = ImGui::GetStyle();
        style.WindowPadding = ImVec2(0.0f, 0.0f);
        ImGui::Begin(ICON_FA_CUBES " Scene", NULL, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        ImVec2 size = ImGui::GetWindowSize();
        if (Window::window->use_imgui_size) {
            Window::window->width = size.x;
            Window::window->height = size.y;
            // glViewport(0, 0, size.x, size.y);
        }

        ImGui::Image((void *)framebufferTextureID, size, ImVec2(0, 1), ImVec2(1, 0));
        if (ImGui::IsWindowHovered()) {
            if (glfwGetMouseButton(Window::window->GetGLFWWindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
                glfwSetInputMode(Window::window->GetGLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                Scene::EditorSceneCamera->first_click = true;
            }

            if (glfwGetMouseButton(Window::window->GetGLFWWindow(), GLFW_MOUSE_BUTTON_2) == GLFW_PRESS) {
                Scene::EditorSceneCamera->Inputs();
            }
        }

        // ImGuizmo Calculations
        if (selected_gameObject) {
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();
            ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, size.x, size.y);

            if (Scene::Main->main_camera_object) {
                glm::mat4 view = Scene::Main->main_camera_object->view;
                glm::mat4 projection = Scene::Main->main_camera_object->projection;

                auto &transform = selected_gameObject->GetComponent<Transform>();
                transform.Update();
                glm::mat4 transformMat = transform.model;
                glm::vec3 originalRot = transform.rotation;
                glm::vec3 originalScal = transform.scale;

                bool snap = ImGui::IsKeyDown(ImGuiKey_LeftCtrl);
                float snapValue = 0.5f;

                if (m_GuizmoMode == ImGuizmo::OPERATION::ROTATE)
                    snapValue = 45.0f;

                float snapValues[3] = {snapValue, snapValue, snapValue};

                ImGuizmo::Manipulate(
                    glm::value_ptr(view), glm::value_ptr(projection),
                    (ImGuizmo::OPERATION)m_GuizmoMode, (ImGuizmo::MODE)m_GuizmoWorld,
                    glm::value_ptr(transformMat), nullptr,
                    snap ? snapValues : nullptr,
                    m_GuizmoMode == ImGuizmo::BOUNDS ? bounds : nullptr,
                    snap ? boundsSnap : nullptr //
                );

                glm::vec3 translation, rotation, scale;
                DecomposeTransform(transformMat, translation, rotation, scale);

                switch (m_GuizmoMode) {
                case ImGuizmo::OPERATION::TRANSLATE: {
                    transform.position = translation;
                }
                case ImGuizmo::OPERATION::ROTATE: {
                    glm::vec3 deltaRot = rotation - originalRot;
                    transform.rotation += deltaRot;
                }
                case ImGuizmo::OPERATION::SCALE: {
                    transform.scale = glm::vec3(scale.x * 2, scale.y * 2, scale.z * 2);
                    break;
                }
                case ImGuizmo::OPERATION::BOUNDS: {
                    transform.position = translation;
                    transform.scale = glm::vec3(scale.x * 2, scale.y * 2, scale.z * 2);
                    break;
                }
                }
            }
        }
        // ImGuizmo Calculations

        // ImGuizmo Modifiers
        ImGui::SetCursorPosY(34);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);

        ImGui::PushStyleColor(
            ImGuiCol_ChildBg,
            (ImVec4)ImColor::HSV(0.0f, 0.0f, 0.0f, 0.6f));
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.5f);
        ImGui::BeginChild("##Gizmo", ImVec2(97 + 34, 35));
        ImVec2 gizmoSize = ImGui::GetWindowSize();
        ImVec2 centerCalc = ImGui::GetCursorPos();
        centerCalc.x += gizmoSize.x / 2;
        centerCalc.y += gizmoSize.y / 2;

        float objOffset = 24;
        float offset = 34;
        float blockSize = 32;

        ImGui::SetCursorPos(ImVec2(5, centerCalc.y - 14));
        if (ImGui::Button(ICON_FA_ARROWS_UP_DOWN_LEFT_RIGHT)) {
            m_GuizmoMode = ImGuizmo::OPERATION::TRANSLATE;
        }

        ImGui::SameLine();
        ImGui::SetCursorPos(ImVec2(5 + (blockSize), centerCalc.y - 12));
        if (ImGui::Button(ICON_FA_ARROWS_ROTATE)) {
            m_GuizmoMode = ImGuizmo::OPERATION::ROTATE;
        }

        ImGui::SameLine();
        ImGui::SetCursorPos(ImVec2(5 + (blockSize * 2), centerCalc.y - 12));
        if (ImGui::Button(ICON_FA_MAXIMIZE)) {
            m_GuizmoMode = ImGuizmo::OPERATION::SCALE;
        }

        ImGui::SameLine();
        ImGui::SetCursorPos(ImVec2(5 + (blockSize * 3), centerCalc.y - 12));
        if (ImGui::Button(ICON_FA_SQUARE)) {
            m_GuizmoMode = ImGuizmo::OPERATION::BOUNDS;
        }

        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();

        {
            ImGui::SetCursorPosY((32 * 2) + 10);
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);

            ImGui::PushStyleColor(
                ImGuiCol_ChildBg,
                (ImVec4)ImColor::HSV(0.0f, 0.0f, 0.0f, 0.6f));
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.5f);
            ImGui::BeginChild("##Gizmo_Mode", ImVec2(97 / 1.4, 32));
            ImVec2 gizmoSize = ImGui::GetWindowSize();
            ImVec2 centerCalc = ImGui::GetCursorPos();
            centerCalc.x += gizmoSize.x / 2;
            centerCalc.y += gizmoSize.y / 2;

            float offset = 30;

            ImGui::SetCursorPos(ImVec2(centerCalc.x + 2 - offset, centerCalc.y - 12));
            if (ImGui::Button(ICON_FA_GLOBE)) {
                m_GuizmoWorld = ImGuizmo::MODE::WORLD;
            }
            if (ImGui::IsItemHovered()) {
                // focusedOnScene = false;
                // hoveredScene = false;
            }
            ImGui::SameLine();
            ImGui::SetCursorPos(ImVec2(centerCalc.x - 26 + offset, centerCalc.y - 12));
            if (ImGui::Button(ICON_FA_SQUARE)) {
                m_GuizmoWorld = ImGuizmo::MODE::LOCAL;
            }
            ImGui::EndChild();
            ImGui::PopStyleColor();
            ImGui::PopStyleVar();
        }

        ImGui::SameLine();

        ImGui::End();
        style.WindowPadding = ImVec2(8.0f, 8.0f);
    }
} // namespace Editor