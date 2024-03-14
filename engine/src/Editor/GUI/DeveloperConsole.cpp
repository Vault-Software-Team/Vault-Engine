#include "Engine/Mono/CSharp.hpp"
#include "Engine/Mono/HelperFunctions.hpp"
#include "Renderer/Stats.hpp"
#include "mono/metadata/class.h"
#include "mono/metadata/object.h"
#include <Editor/GUI/MainGUI.hpp>
#include <Engine/Scene.hpp>
#include <Engine/GameObject.hpp>
#include <Engine/Components/IncludeComponents.hpp>
#include <icons/icons.h>
#include <imgui/imgui.h>
#include <Renderer/Window.hpp>
#include <ImGuizmo/ImGuizmo.h>
#include <Engine/SceneSerialization.hpp>
#include <Engine/Runtime.hpp>
#include <Engine/Audio.hpp>
#include <Engine/Mono/HelperFunctions.hpp>

using namespace Engine;
using namespace Engine::Components;
using namespace VaultRenderer;

namespace Editor {
    DLL_API bool GUI::isDevConOpen;

    bool ccode_StartsWith(const char *a, const char *b) {
        return strncmp(a, b, strlen(b)) == 0;
    }

    std::vector<std::string> console_GetArguments(std::string string) {
        std::vector<std::string> out;
        std::string s;
        std::stringstream ss(string);
        while (std::getline(ss, s, ' ')) {
            out.push_back(s);
        }
        out.erase(out.begin());

        return out;
    }

    std::vector<std::string> console_GetArgumentsWithStarter(std::string string) {
        std::vector<std::string> out;
        std::string s;
        std::stringstream ss(string);
        while (std::getline(ss, s, ' ')) {
            out.push_back(s);
        }

        return out;
    }

    void DeveloperConsole_ExecuteCommand(std::string command) {
        if (ccode_StartsWith(command.c_str(), "exit")) {
            exit(0);
        } else if (ccode_StartsWith(command.c_str(), "warning")) {
            std::vector<std::string> out = console_GetArguments(command);

            std::string real_string;
            for (auto &string : out) {
                real_string += string;
                real_string += " ";
            }
            GUI::LogWarning(real_string);
        } else if (ccode_StartsWith(command.c_str(), "error")) {
            std::vector<std::string> out = console_GetArguments(command);

            std::string real_string;
            for (auto &string : out) {
                real_string += string;
                real_string += " ";
            }
            GUI::LogError(real_string);
        } else if (ccode_StartsWith(command.c_str(), "print")) {
            std::stringstream ss(command);

            std::vector<std::string> out = console_GetArguments(command);
            std::string real_string;
            for (auto &string : out) {
                real_string += string;
                real_string += " ";
            }
            GUI::LogInfo(real_string);
        } else if (ccode_StartsWith(command.c_str(), "success")) {
            std::stringstream ss(command);

            std::vector<std::string> out = console_GetArguments(command);
            std::string real_string;
            for (auto &string : out) {
                real_string += string;
                real_string += " ";
            }
            GUI::LogTick(real_string);
        } else if (ccode_StartsWith(command.c_str(), "clear")) {
            GUI::logs.clear();
        } else {
            // C# STUFF !!
            // auto &klass = CSharp::instance->command_classes[".MyCommand"];
            // std::vector<std::string> out = console_GetArgumentsWithStarter(command);

            // std::string command = out[0];

            // out.erase(out.begin());
            // std::string args;
            // for (auto &string : out) {
            //     args += string;
            //     args += " ";
            // }

            // auto &registry = CSharp::instance->command_classes["DevConsole.CommandRegistry"];

            // void *__args[2] = {
            //     CSharpHelper::StrToMonoString(command),
            //     CSharpHelper::StrToMonoString(args) //
            // };
            // MonoObject *exception = nullptr;
            // mono_runtime_invoke(registry->GetMethod("Execute", 2), registry->GetHandleTarget(), __args, &exception);

            // if (exception) {
            //     MonoObject *exc = NULL;
            //     MonoString *str = mono_object_to_string(exception, &exc);
            //     if (exc) {
            //         mono_print_unhandled_exception(exc);
            //     } else {
            //         Editor::GUI::LogError(mono_string_to_utf8(str)); // Log log(mono_string_to_utf8(str), LOG_ERROR);
            //     }
            // }
        }
    }

    void GUI::DeveloperConsole() {
        static std::string command = "";

        ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
        if (ImGui::IsKeyPressed(ImGuiKey_GraveAccent)) {
            isDevConOpen = !isDevConOpen;
        }
        if (!isDevConOpen) return;
        if (ImGui::Begin(ICON_FA_TERMINAL " Developer Console", &isDevConOpen, ImGuiWindowFlags_NoDocking)) {
            ImVec2 console_size = ImGui::GetWindowSize();
            ImGui::BeginChild("Dev Logs", ImVec2(console_size.x, console_size.y - 70));

            for (auto &log : GUI::logs) {
                switch (log.type) {
                case Log::LOG_INFO:
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
                                       ICON_FA_MESSAGE);
                    // same line
                    ImGui::SameLine();
                    ImGui::TextWrapped("%s",
                                       (std::string("  ") + log.content).c_str());
                    break;
                case Log::LOG_WARNING:
                    ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.0f, 1.0f),
                                       ICON_FA_TRIANGLE_EXCLAMATION);
                    // same line
                    ImGui::SameLine();
                    ImGui::TextWrapped("%s",
                                       (std::string("  ") + log.content).c_str());
                    break;
                case Log::LOG_ERROR:
                    ImGui::TextColored(ImVec4(1.2f, 0.0f, 0.0f, 1.0f),
                                       ICON_FA_CIRCLE_EXCLAMATION);
                    // same line
                    ImGui::SameLine();
                    ImGui::TextWrapped("%s",
                                       (std::string("  ") + log.content).c_str());
                    break;
                case Log::LOG_TICK:
                    ImGui::TextColored(ImVec4(0.019f, 0.988f, 0.08627f, 1),
                                       ICON_FA_CIRCLE_CHECK);
                    // same line
                    ImGui::SameLine();
                    ImGui::TextWrapped("%s",
                                       (std::string("  ") + log.content).c_str());
                    break;
                }
            }

            ImGui::SetScrollHereY(0.999f);
            ImGui::EndChild();
            ImGui::PushItemWidth(console_size.x);
            ImGui::InputText("##dev_input", &command);

            if (ImGui::IsKeyPressed(ImGuiKey_Enter)) {
                DeveloperConsole_ExecuteCommand(command);
                command = "";
            }
        }
        ImGui::End();
    }
} // namespace Editor