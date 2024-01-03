#include <Editor/GUI/MainGUI.hpp>
#include <Engine/Components/IncludeComponents.hpp>
#include <icons/icons.h>
#include "../vendor/glm/ext.hpp"
#include "../vendor/glm/glm.hpp"
#include "../vendor/glm/gtc/matrix_transform.hpp"
#include "../vendor/glm/gtc/type_ptr.hpp"
#include "../vendor/glm/gtx/quaternion.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "../vendor/glm/gtx/matrix_decompose.hpp"
#include "../vendor/glm/gtx/rotate_vector.hpp"
#include "../vendor/glm/gtx/vector_angle.hpp"

using namespace Engine::Components;

namespace Editor {
    DLL_API std::vector<Log> GUI::logs;
    DLL_API uint32_t GUI::framebufferTextureID;
    DLL_API Engine::GameObject *GUI::selected_gameObject;
    DLL_API std::string GUI::dragPayload;

    char asciitolower(char in) {
        if (in <= 'Z' && in >= 'A')
            return in - ('Z' - 'z');
        return in;
    }

    void GUI::SetNameIcon(std::string &txt, Engine::GameObject *gameObject) {
        if (gameObject->HasComponent<Camera>()) {
            txt = ICON_FA_CAMERA;
            return;
        }

        if (gameObject->HasComponent<AmbientLight>()) {
            txt = ICON_FA_STAR;
            return;
        }

        if (gameObject->HasComponent<DirectionalLight>()) {
            txt = ICON_FA_SUN;
            return;
        }

        if (gameObject->HasComponent<PointLight>()) {
            txt = ICON_FA_LIGHTBULB;
            return;
        }

        if (gameObject->HasComponent<Text3D>()) {
            txt = ICON_FA_FONT;
            return;
        }

        txt = ICON_FA_CUBE;
    }

    void GUI::DrawVec3Control(const std::string &label, glm::vec3 &values, float resetValue, float columnWidth) {
        ImGui::PushID(label.c_str());
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, 88);
        ImGui::Text("%s", label.c_str());
        ImGui::NextColumn();

        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.5f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImVec2 buttonSize = {lineHeight + 2.0f, lineHeight};

        // disable button rounded corners
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                              ImVec4{0.9f, 0.2f, 0.2f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                              ImVec4{0.8f, 0.1f, 0.15f, 1.0f});

        if (ImGui::Button("X", buttonSize)) {
            values.x = resetValue;
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                              ImVec4{0.3f, 0.8f, 0.3f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                              ImVec4{0.2f, 0.7f, 0.2f, 1.0f});

        if (ImGui::Button("Y", buttonSize)) {
            values.y = resetValue;
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                              ImVec4{0.2f, 0.35f, 0.9f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                              ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
        if (ImGui::Button("Z", buttonSize)) {
            values.z = resetValue;
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();
        ImGui::Columns(1);
        ImGui::PopStyleVar(3);
        ImGui::PopID();
    }

    void GUI::DrawVec2Control(const std::string &label, glm::vec2 &values, float resetValue, float columnWidth) {
        ImGui::PushID(label.c_str());
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, 100);
        ImGui::Text(label.c_str());
        ImGui::NextColumn();

        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.5f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

        float lineHeight =
            GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImVec2 buttonSize = {lineHeight + 2.0f, lineHeight};

        // disable button rounded corners
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                              ImVec4{0.9f, 0.2f, 0.2f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                              ImVec4{0.8f, 0.1f, 0.15f, 1.0f});

        if (ImGui::Button("X", buttonSize)) {
            values.x = resetValue;
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                              ImVec4{0.3f, 0.8f, 0.3f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                              ImVec4{0.2f, 0.7f, 0.2f, 1.0f});

        if (ImGui::Button("Y", buttonSize)) {
            values.y = resetValue;
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::Columns(1);
        ImGui::PopStyleVar(3);
        ImGui::PopID();
    }

    bool GUI::IsImage(const std::string &file_path) {
        if (file_path.ends_with(".png") || file_path.ends_with(".jpg") || file_path.ends_with(".jpeg") || file_path.ends_with(".tiff") || file_path.ends_with(".bmp")) {
            return true;
        }

        return false;
    }

    bool GUI::DecomposeTransform(const glm::mat4 &transform, glm::vec3 &translation,
                                 glm::vec3 &rotation, glm::vec3 &scale) {
        // From glm::decompose in matrix_decompose.inl

        using namespace glm;
        using T = float;

        mat4 LocalMatrix(transform);

        // Normalize the matrix.
        if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0),
                         epsilon<T>()))
            return false;

        // First, isolate perspective.  This is the messiest.
        if (epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0),
                            epsilon<T>()) ||
            epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0),
                            epsilon<T>()) ||
            epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0),
                            epsilon<T>())) {
            // Clear the perspective partition
            LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] =
                static_cast<T>(0);
            LocalMatrix[3][3] = static_cast<T>(1);
        }

        // Next take care of translation (easy).
        translation = vec3(LocalMatrix[3]);
        LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

        vec3 Row[3], Pdum3;

        // Now get scale and shear.
        for (length_t i = 0; i < 3; ++i)
            for (length_t j = 0; j < 3; ++j)
                Row[i][j] = LocalMatrix[i][j];

        // Compute X scale factor and normalize first row.
        scale.x = length(Row[0]);
        Row[0] = detail::scale(Row[0], static_cast<T>(1));
        scale.y = length(Row[1]);
        Row[1] = detail::scale(Row[1], static_cast<T>(1));
        scale.z = length(Row[2]);
        Row[2] = detail::scale(Row[2], static_cast<T>(1));

// At this point, the matrix (in rows[]) is orthonormal.
// Check for a coordinate system flip.  If the determinant
// is -1, then negate the matrix and the scaling factors.
#if 0
                        Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
                    if (dot(Row[0], Pdum3) < 0)
                    {
                        for (length_t i = 0; i < 3; i++)
                        {
                            scale[i] *= static_cast<T>(-1);
                            Row[i] *= static_cast<T>(-1);
                        }
                    }
#endif

        rotation.y = asin(-Row[0][2]);
        if (cos(rotation.y) != 0) {
            rotation.x = atan2(Row[1][2], Row[2][2]);
            rotation.z = atan2(Row[0][1], Row[0][0]);
        } else {
            rotation.x = atan2(-Row[2][0], Row[1][1]);
            rotation.z = 0;
        }

        return true;
    }
} // namespace Editor