#pragma once
#include <string>

namespace VaultRenderer {
    class Statistics {
    public:
        static inline int draw_calls;
        static inline std::string vendor;
        static inline std::string renderer;
        static inline std::string version;
        static inline std::string shading_language;
        static inline int fps;

        static void SetStats();
        static void ResetDrawCalls();
        static void DrawCall();
        static int GetDrawCalls();

        // ImGui::Text("Vendor: %s", glGetString(GL_VENDOR));
        // ImGui::Text("Renderer: %s", glGetString(GL_RENDERER));
        // ImGui::Text("Version: %s", glGetString(GL_VERSION));
        // ImGui::Text("Shading Language Version: %s",
        //             glGetString(GL_SHADING_LANGUAGE_VERSION));
    };
} // namespace VaultRenderer