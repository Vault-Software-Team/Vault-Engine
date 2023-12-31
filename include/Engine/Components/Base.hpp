#pragma once
#include <dllapi.hpp>
#include <entt/entt.hpp>
#include <icons/icons.h>
#include <imgui/imgui_stdlib.h>

namespace Engine {
    namespace Components {
        struct DLL_API Base {
            template <typename T, typename GUI>
            void DrawComponent(entt::registry &registry, float offset, GUI uiCall) {
                bool removed = false;
                if (ImGui::BeginPopup(("ComponentSettings_" + T::display_name).c_str())) {
                    ImGui::Text("%s", (T::display_name + " Settings").c_str());
                    if (ImGui::Button(ICON_FA_TRASH " Remove Component", ImVec2(200, 0))) {
                        removed = true;
                        registry.remove<T>(entity);
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }

                const ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth;
                bool treeNodeOpen = ImGui::TreeNodeEx(typeid(T).name(), flags, T::display_name.c_str());
                ImGui::SameLine(ImGui::GetWindowWidth() - offset);
                if (ImGui::Button(ICON_FA_GEAR, ImVec2(20, 20))) {
                    ImGui::OpenPopup(("ComponentSettings_" + T::display_name).c_str());
                }
                if (treeNodeOpen) {
                    if (!removed)
                        uiCall();
                    ImGui::TreePop();
                }
            }

            entt::entity entity;
            std::string ID;
            static inline const std::string display_name = "Base Component";
            bool static_registry = false;

            virtual void OnGUI(){};
            virtual void Update() {}
            virtual void Init() {}
        };
    } // namespace Components
} // namespace Engine