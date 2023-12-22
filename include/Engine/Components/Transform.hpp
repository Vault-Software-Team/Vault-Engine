#pragma once
#include <glm/ext.hpp>
#include <glm/gtx/quaternion.hpp>
#include "Base.hpp"

namespace Engine {
    namespace Components {
        struct Transform : Base {
            glm::vec3 position;
            glm::vec3 rotation; // in radians
            glm::vec3 scale;
            glm::mat4 model;
            glm::vec3 forward;
            glm::vec3 right;
            glm::vec3 up;

            Transform() = default;

            glm::mat4 &UpdateModel();
        };
    } // namespace Components
} // namespace Engine