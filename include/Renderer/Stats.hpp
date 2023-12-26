#pragma once
#include <dllapi.hpp>
#include <string>

namespace VaultRenderer {
    class DLL_API Statistics {
    public:
        static DLL_API int draw_calls;
        static DLL_API std::string vendor;
        static DLL_API std::string renderer;
        static DLL_API std::string version;
        static DLL_API std::string shading_language;
        static DLL_API int fps;

        static DLL_API void SetStats();
        static DLL_API void ResetDrawCalls();
        static DLL_API void DrawCall();
        static DLL_API int GetDrawCalls();

        // ImGui::Text("Vendor: %s", glGetString(GL_VENDOR));
        // ImGui::Text("Renderer: %s", glGetString(GL_RENDERER));
        // ImGui::Text("Version: %s", glGetString(GL_VERSION));
        // ImGui::Text("Shading Language Version: %s",
        //             glGetString(GL_SHADING_LANGUAGE_VERSION));
    };
} // namespace VaultRenderer