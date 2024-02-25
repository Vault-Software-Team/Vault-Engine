#pragma once
#include <string>
#include <unordered_map>

namespace Engine {
    class Input {
    public:
        static std::unordered_map<int, bool> keys;

        static bool IsKeyPressed(int key);
        static bool IsKeyReleased(int key);
        static bool IsKeyDown(int key);
        static bool IsKeyUp(int key);
    };
} // namespace Engine