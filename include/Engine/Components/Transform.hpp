#pragma once
#include <glm/ext.hpp>
#include "Base.hpp"
#include <dllapi.hpp>

namespace Engine {
    namespace Components {
        struct DLL_API Transform : Base {
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
        };
    } // namespace Components
} // namespace Engine