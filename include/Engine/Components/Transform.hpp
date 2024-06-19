#pragma once
#include <dllapi.hpp>
#include <glm/ext.hpp>
#include "Base.hpp"

namespace Engine {
    namespace Components {
        struct DLL_API Transform : Base {
            static inline const std::string display_name = "Transform";
            glm::vec3 position = glm::vec3(0, 0, 0);
            glm::vec3 rotation = glm::vec3(0, 0, 0); // in radians
            glm::vec3 scale = glm::vec3(1, 1, 1);
            glm::mat4 model;
            glm::vec3 forward;
            glm::vec3 right;
            glm::vec3 up;

            Transform() = default;

            glm::mat4 &UpdateModel();
            void Update() override;
            void OnGUI() override;

            void LookAt(glm::vec3 target);

            void Translate(glm::vec3 translation);
            void Rotate(glm::vec3 rotation);
            void Scale(glm::vec3 scale);
        };
    } // namespace Components
} // namespace Engine