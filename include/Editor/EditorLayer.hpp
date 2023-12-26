#pragma once
#include <dllapi.hpp>

namespace Editor {
    class DLL_API EditorLayer {
    public:
        static inline EditorLayer *instance;

        EditorLayer();
        void GUI();
    };
} // namespace Editor