#pragma once
#include <dllapi.hpp>
#include <glm/ext.hpp>
#include "Base.hpp"
#include <Renderer/Mesh.hpp>
#include <Renderer/Shader.hpp>
#include <memory>
#include <Renderer/Texture.hpp>
#include <Engine/Model.hpp>
#include <thread>

namespace Engine {
    namespace Components {
        struct DLL_API BoneManipulator : Base {
            static inline const std::string display_name = "Bone Manipulator";
            std::string nodeName = "";

            BoneManipulator() = default;
        };
    } // namespace Components
} // namespace Engine