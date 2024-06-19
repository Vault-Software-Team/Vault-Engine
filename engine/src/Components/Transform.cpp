#include <Engine/Components/Transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <Engine/GameObject.hpp>
#include <icons/icons.h>

#include <Editor/GUI/MainGUI.hpp>

namespace Engine::Components {
    glm::mat4 &Transform::UpdateModel() {
        if (std::isnan(position.x))
            position.x = 0;
        if (std::isnan(position.y))
            position.y = 0;
        if (std::isnan(position.z))
            position.z = 0;

        if (std::isnan(rotation.x))
            rotation.x = 0;
        if (std::isnan(rotation.y))
            rotation.y = 0;
        if (std::isnan(rotation.z))
            rotation.z = 0;

        if (std::isnan(scale.x))
            scale.x = 1;
        if (std::isnan(scale.y))
            scale.y = 1;
        if (std::isnan(scale.z))
            scale.z = 1;

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
        DrawComponent<Transform>(Scene::Main->EntityRegistry, 40, [&] {
            Editor::GUI::DrawVec3Control("Position", position);
            rotation = glm::degrees(rotation);
            Editor::GUI::DrawVec3Control("Rotation", rotation);
            rotation = glm::radians(rotation);
            Editor::GUI::DrawVec3Control("Scale", scale, 1.0);

            // Have to add this line so the next options thing on the next component will be on a correct place, quite stupid
            ImGui::NewLine();
        });
    }

    void Transform::LookAt(glm::vec3 target) {
        glm::vec3 direction = glm::normalize(target - position);
        rotation = glm::eulerAngles(
            glm::quatLookAt(direction, glm::vec3(0, 1, 0)));
    }

    void Transform::Translate(glm::vec3 translation) { position += translation; }

    void Transform::Rotate(glm::vec3 rotation) { this->rotation += rotation; }

    void Transform::Scale(glm::vec3 scale) { this->scale += scale; }
} // namespace Engine::Components