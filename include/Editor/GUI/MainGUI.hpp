#pragma once
#include "imgui/TextEditor.hpp"
#include <dllapi.hpp>
#include <Engine/GameObject.hpp>
#include <string>
#include <vector>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/imgui_internal.h>
#include <Renderer/Mesh.hpp>

namespace Editor {
    char asciitolower(char in);
    struct DLL_API Log {
        enum LogType {
            LOG_INFO,
            LOG_WARNING,
            LOG_ERROR,
            LOG_TICK
        } type;
        std::string content;
    };

    class DLL_API GUI {

    public:
        static DLL_API DLL_API std::vector<Log> logs;
        static DLL_API std::string dragPayload;
        static DLL_API std::string TextEditor_text;
        static DLL_API std::string TextEditor_path;
        static DLL_API TextEditor *text_editor;
        static DLL_API bool isDevConOpen;

        static DLL_API Engine::GameObject *selected_gameObject;
        static DLL_API uint32_t framebufferTextureID;
        static DLL_API void LogInfo(const std::string &content);
        static DLL_API void LogTick(const std::string &content);
        static DLL_API void LogError(const std::string &content);
        static DLL_API void LogWarning(const std::string &content);

        static DLL_API void MenuBar(); // AKA the top bar you see on applications
        static DLL_API void SetNameIcon(std::string &txt, ImVec4 &color, Engine::GameObject *gameObject);
        static DLL_API void Hierarchy();
        static DLL_API void Assets();
        static DLL_API void Console();
        static DLL_API void Scene();
        static DLL_API void Components();
        static DLL_API void Config();
        static DLL_API void BottomLogCounter();
        static DLL_API void DeveloperConsole();

        static DLL_API bool isMaterialInspectorOpen;
        static DLL_API bool isConfigurationOpen;
        static DLL_API VaultRenderer::Material MaterialInspector_material;
        static DLL_API std::string MaterialInspector_material_path;
        static DLL_API void MaterialInspector();

        static DLL_API void DrawVec3Control(const std::string &label, glm::vec3 &values, float resetValue = 0.0f, float columnWidth = 100.0f);
        static DLL_API void DrawVec2Control(const std::string &label, glm::vec2 &values, float resetValue = 0.0f, float columnWidth = 100.0f);

        static DLL_API bool IsImage(const std::string &file_path);

        static DLL_API bool DecomposeTransform(const glm::mat4 &transform, glm::vec3 &translation, glm::vec3 &rotation, glm::vec3 &scale);

        // GAME GUI
        static DLL_API void Game_Scene();
        static DLL_API void InitTextEditor();
    };
} // namespace Editor