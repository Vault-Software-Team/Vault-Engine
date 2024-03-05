#pragma once
#include <dllapi.hpp>

namespace Editor {
    class DLL_API EditorLayer {
    public:
        static DLL_API EditorLayer *instance;

        EditorLayer();
        void GUI();
        void GameGUI();
    };
} // namespace Editor