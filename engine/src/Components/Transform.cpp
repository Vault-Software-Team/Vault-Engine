#include <Engine/Components/Transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <Engine/GameObject.hpp>
#include <icons/icons.h>

#include <Editor/GUI/MainGUI.hpp>

namespace Engine::Components {
    glm::mat4 &Transform::UpdateModel() {
        model = glm::translate(glm::mat4(1.0f), position) *
                glm::toMat4(glm::quat(rotation)) *
                glm::scale(glm::mat4(1.0f),
                           glm::vec3(scale.x * 0.5, scale.y * 0.5,
                                     scale.z * 0.5));

        glm::mat4 inverted = glm::inverse(model);
        forward = normalize(glm::vec3(inverted[2]));

        right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));
        up = glm::normalize(glm::cross(right, forward));

        auto &self = GameObject::FindGameObjectByEntity(entity);
        if (self->parent != "NO_PARENT") {
            auto &pTransform = GameObject::FindGameObjectByID(self->parent)->GetComponent<Transform>();
            pTransform.Update();
            model = pTransform.model * model;
        }
        return model;
    }

    void Transform::Update() {
        UpdateModel();
    }

    void Transform::OnGUI() {
        if (ImGui::TreeNode("Transform")) {
            Editor::GUI::DrawVec3Control("Position", position);
            Editor::GUI::DrawVec3Control("Rotation", rotation);
            Editor::GUI::DrawVec3Control("Scale", scale);
            ImGui::TreePop();
        }
    }
} // namespace Engine::Components