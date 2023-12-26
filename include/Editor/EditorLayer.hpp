#pragma once

namespace Editor {
    class EditorLayer {
    public:
        static inline EditorLayer *instance;

        EditorLayer();
        void GUI();
    };
} // namespace Editor