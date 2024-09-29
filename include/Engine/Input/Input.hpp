#pragma once
#include "glm/fwd.hpp"
#include <string>
#include <unordered_map>

namespace Engine {
    class Input {
    public:
        static std::unordered_map<int, bool> keys;

        static glm::vec2 winPos;

        static bool IsKeyPressed(int key);
        static bool IsKeyReleased(int key);
        static bool IsKeyDown(int key);
        static bool IsKeyUp(int key);

        static int GetMouseXAxis();
        static int GetMouseYAxis();

        static int GetVerticalAxis();
        static int GetHorizontalAxis();
    };
} // namespace Engine