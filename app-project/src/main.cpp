/*
Welcome to the worst game engines codes main file!
In here you will throw up all over your desk cuz the coding is fucking ass
*/

#include <stb_image/stb_image.h>
#include "Engine/Audio.hpp"
#include "Engine/CascadedShadowMap.hpp"
#include "Engine/Components/MeshRenderer.hpp"
#include "Engine/Components/Transform.hpp"
#include "Engine/Discord.hpp"
#include "Engine/GameObject.hpp"
#include "Engine/HDRSkybox.hpp"
#include "Engine/Model.hpp"
#include "Engine/SimpleCalls.hpp"
#include "ExternalApps/Links.hpp"
#include "GLFW/glfw3.h"
#include "HyperScript/HyperScript.hpp"
#include "Renderer/Framebuffer.hpp"
#include "icons/icons.h"
#include "imgui/TextEditor.hpp"
#include "json.hpp"
#include "mono/metadata/row-indexes.h"
#include <filesystem>
#include <iostream>
#include <Renderer/Window.hpp>
#include <Renderer/Mesh.hpp>
#include <Renderer/Texture.hpp>
#include <Renderer/Skybox.hpp>
#include <imgui/imgui.h>
#include <Engine/Scene.hpp>
#include <Engine/Components/IncludeComponents.hpp>
#include <Renderer/ShadowMap.hpp>
#include <Renderer/Stats.hpp>
#include <Renderer/Font.hpp>
#include <Engine/Runtime.hpp>
#include <Editor/EditorLayer.hpp>
#include <fstream>
#include <Editor/GUI/MainGUI.hpp>
#include <ImGuiFileDialog/ImGuiFileDialog.h>
#include <Engine/Batch.hpp>
#include <Engine/SceneSerialization.hpp>
#include <Engine/Scripting/LoadScripts.hpp>
#include <script_test.hpp>
#include <experimental/filesystem>
#include <thread>

namespace fs = std::filesystem;
static VaultRenderer::Shader *default_shader;

using namespace Engine;
using namespace VaultRenderer;
using namespace Engine::Components;

#include <Engine/Mono/CSharp.hpp>

void AspectRatioCameraViewport() {
    const int targetWidth = 1920, targetHeight = 1080;
    float targetAspectRatio = (float)targetWidth / (float)targetHeight;

    int aspectWidth = VaultRenderer::Window::window->width;
    int aspectHeight = (int)((float)aspectWidth / targetAspectRatio);
    if (aspectHeight > VaultRenderer::Window::window->height) {
        aspectHeight = VaultRenderer::Window::window->height;
        aspectWidth = (int)((float)aspectHeight * targetAspectRatio);
    }
    int vpX = (int)(((float)VaultRenderer::Window::window->width / 2.0f) - ((float)aspectWidth / 2.0f));
    int vpY = (int)(((float)VaultRenderer::Window::window->height / 2.0f) - ((float)aspectHeight / 2.0f));

    if (Engine::Scene::Main->main_camera_object) {
        Engine::Scene::Main->main_camera_object->width = 1920;
        Engine::Scene::Main->main_camera_object->height = 1080;
    }

    glViewport(vpX, vpY, aspectWidth, aspectHeight);
}

void OnGUI(uint32_t smID) {
    using namespace VaultRenderer;
    using namespace Engine;
    using namespace Engine::Components;
    // ImGui::Begin("Statistics");
    // ImGui::Text("Draw Calls: %d", Statistics::GetDrawCalls());
    // ImGui::Text("Vendor: %s", Statistics::vendor.c_str());
    // ImGui::Text("Renderer: %s", Statistics::renderer.c_str());
    // ImGui::Text("Version: %s", Statistics::version.c_str());
    // ImGui::Text("Shading Language: %s", Statistics::shading_language.c_str());
    // ImGui::End();
}

using namespace Editor;
void print(const std::string &traki) {
    std::cout << traki << "\n";
}
fs::path path;
int selected = -1;
static const bool DEBUG_MODE = true;
static const std::string GAME_ENGINE_BINARY = DEBUG_MODE ? "./build/app/app" : "./bin/build.out";

void DisplayProject(GLFWwindow *window, const std::string &name,
                    const std::string &path, int index) {
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.05f, 0.055f, 0.051f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                          ImVec4(0.07f, 0.075f, 0.071f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                          ImVec4(0.1f, 0.105f, 0.11f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.03f, 0.5f));
    ImGui::SetCursorPosX(25);
    if (ImGui::Button(std::string(name + "\n" + path).c_str(),
                      ImVec2(ImGui::GetWindowWidth() - 50, 70))) {
#ifdef _WIN32
        // open build.exe
        // get cwd
        char cwd[1024];
        _getcwd(cwd, sizeof(cwd));
        std::string s_Cwd = cwd;

        std::cout << s_Cwd + "\\bin\\win_build.exe \"" + path + "\" \"" + CsharpVariables::oldCwd + "\\dotnet\\dotnet\"" << std::endl;
        std::thread t(
            [&]() { system((".\\bin\\win_build.exe \"" + path + "\" \"" + +CsharpVariables::oldCwd + "\\dotnet\\dotnet\"").c_str()); });

        t.detach();
#else
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        std::string s_Cwd = cwd;
        std::string command = GAME_ENGINE_BINARY + " \"" + path + "\"";
        std::cout << command << "\n";

        std::thread t([&]() {
            std::string command = GAME_ENGINE_BINARY + " \"" + path + "\"";
            int a = system(command.c_str());
            std::cout << "[THREAD] a = " << a << " | command: " << command << "\n";
        });
        t.detach();
#endif
    }

    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1)) {
        selected = index;
        /// as
        /// as
        ImGui::OpenPopup("Project Options");
    }

    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar();

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
}

struct Project {
    std::string name;
    std::string path;
};
static fs::path CONFIG_FILE = "";

void SaveProjectsConfig(const std::vector<Project> &projects) {
    nlohmann::json j_projects = nlohmann::json::array();
    for (auto &project : projects) {
        nlohmann::json j;
        j["name"] = project.name;
        j["path"] = project.path;
        j_projects.push_back(j);
    }

    std::ofstream file(CONFIG_FILE / "projects.json");
    file << j_projects;
}

int main() {
    using namespace VaultRenderer;

    Window window(1280, 720, "Vault Engine | Project Selector");
    window.SetWindowIcon("./editor/icons/project_icon.png");

    float timestep = 0;

    Texture logo("./editor/icons/logo_github.png", TEXTURE_GUI_ICON);

    fs::path config_path;
#ifdef _WIN32
    config_path = std::string(getenv("APPDATA"));
    config_path = config_path.parent_path() / "Roaming" / "Vault Engine";
    if (!fs::exists(config_path)) {
        fs::create_directories(config_path);

        std::ofstream fout(config_path / "projects.json");
        fout << "[]";
        fout.close();
    }
#else
    // get HOME env
    fs::path home(getenv("HOME"));
    // create path
    config_path = home / ".vault_engine";
    if (!fs::exists(config_path))
        fs::create_directories(config_path);

    if (!fs::exists(config_path / "projects.json")) {
        std::ofstream fout(config_path / "projects.json");
        fout << "[]";
        fout.close();
    }
    CONFIG_FILE = config_path;
#endif

    std::vector<Project> projects;
    nlohmann::json j_Projects = nlohmann::json::parse(std::fstream(config_path / "projects.json"));
    for (auto &project : j_Projects) {
        projects.push_back({project["name"], project["path"]});
    }

    std::cout << config_path.string() << "\n";

    ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeFile, "", ImVec4(1, 1, 1, 1.0f), ICON_FA_FILE);
    ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeDir, "", ImVec4(1, 1, 1, 1.0f), ICON_FA_FOLDER);

    ImGuiStyle &style = ImGui::GetStyle();
    style.WindowRounding = 0.f;

    auto Function_GUI = [&]() {
        int width, height;
        glfwGetWindowSize(window.GetGLFWWindow(), &width, &height);

        const ImGuiViewport *viewport = ImGui::GetMainViewport();

        // Set position to the bottom of the viewport
        // ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y));
        // // ImGui::SetNextWindowPos(ImVec2(0, 0));
        // ImGui::SetNextWindowSize(ImVec2(width, height));
        // flags
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse |
                                 ImGuiWindowFlags_NoResize |
                                 ImGuiWindowFlags_NoMove |
                                 ImGuiWindowFlags_NoBringToFrontOnFocus |
                                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDocking;

        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y));
        ImGui::SetNextWindowSize(ImVec2(width - 250, height));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(25, 25));
        ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
        if (ImGui::Begin("Mainpanel", nullptr, flags)) {
            if (ImGui::BeginChild("##icon stuff", ImVec2(0, 200))) {
                ImGui::Image((void *)logo.texture_data->ID, ImVec2(340, 200), ImVec2(0, 1), ImVec2(1, 0));
                ImGui::EndChild();
            }

            int i = 0;
            for (auto &project : projects) {
                DisplayProject(window.GetGLFWWindow(), project.name, project.path, i);
                i++;
            }

            if (ImGui::BeginPopup("Project Options")) {
                if (selected < 0) ImGui::CloseCurrentPopup();
                if (ImGui::Button(ICON_FA_TRASH_CAN " Delete Project", ImVec2(200, 25))) {
                    projects.erase(projects.begin() + selected);
                    SaveProjectsConfig(projects);
                    ImGui::CloseCurrentPopup();
                }
                if (ImGui::Button("Close Popup", ImVec2(200, 25))) ImGui::CloseCurrentPopup();

                ImGui::EndPopup();
            }
        }

        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x + (width - 250), viewport->Pos.y));
        ImGui::SetNextWindowSize(ImVec2(250, height));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(25, 25));
        ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
        if (ImGui::Begin("Sidepanel", nullptr, flags)) {
            if (ImGui::Button("Create Project", ImVec2(200, 60))) {
                ImGui::OpenPopup("CreateProject_Popup");
            }
            if (ImGui::Button("Open Project", ImVec2(200, 60))) {
                ImGuiFileDialog::Instance()->OpenDialog("OpenProject", ICON_FA_FOLDER " Choose a project to open ..", NULL, ".");
            }

            if (ImGui::Button("Install .NET", ImVec2(200, 60))) {
                External::OpenURLInBrowser("https://dotnet.microsoft.com/en-us/download/dotnet/6.0");
            }
            ImGui::Text(".NET 6.0 or higher required!");

            static char name[256];

            if (ImGuiFileDialog::Instance()->Display("NewProject")) {
                //         // action if OK
                if (ImGuiFileDialog::Instance()->IsOk()) {
                    std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                    std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
                    std::ofstream projFile(filePath + "/settings.vault.project");

                    nlohmann::json data;
                    data["name"] = name;
                    data["path"] = filePathName;

                    projFile << data.dump(4);
                    projFile.close();

                    projects.push_back({name, filePathName});
                    fs::path projectPath = filePathName;

                    SaveProjectsConfig(projects);

                    // copy assets, shaders, build and imgui.ini to the
                    // project
                    std::cout << "COPYING FILES...\n";
                    fs::copy(fs::absolute("assets"), projectPath / "assets", fs::copy_options::recursive);
                    std::cout << "COPYING FILES 1...\n";
                    fs::copy(fs::absolute("shaders"), projectPath / "shaders", fs::copy_options::recursive);
                    std::cout << "COPYING FILES 2...\n";
                    fs::copy(fs::absolute("editor"), projectPath / "editor", fs::copy_options::recursive);
                    std::cout << "COPYING FILES 3...\n";
                    fs::copy(fs::absolute("fonts"), projectPath / "fonts", fs::copy_options::recursive);
                    std::cout << "COPYING FILES 4...\n";
                    fs::copy_file(fs::absolute("imgui.ini"), projectPath / "imgui.ini", fs::copy_options::recursive);
                }

                ImGuiFileDialog::Instance()->Close();
            }

            if (ImGuiFileDialog::Instance()->Display("OpenProject")) {
                // action if OK
                if (ImGuiFileDialog::Instance()->IsOk()) {
                    std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                    std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

                    if (fs::exists(fs::path(filePathName) / "settings.vault.project")) {
                        std::ifstream projFile(filePathName + "/settings.vault.project");
                        nlohmann::json data = nlohmann::json::parse(projFile);
                        projFile.close();

                        projects.push_back({data["name"], data["path"]});
                        SaveProjectsConfig(projects);
                    }
                }

                ImGuiFileDialog::Instance()->Close();
            }

            if (ImGui::BeginPopup("CreateProject_Popup")) {
                ImGui::InputText("Name", name, 256);
                if (ImGui::Button(ICON_FA_FOLDER " Select Directory"))
                    ImGuiFileDialog::Instance()->OpenDialog("NewProject", ICON_FA_FOLDER " Choose a folder to create project in ..", NULL, ".");

                ImGui::EndPopup();
            }
        }
        ImGui::End();

        ImGuiWindowFlags flags2 = ImGuiWindowFlags_NoCollapse |
                                  ImGuiWindowFlags_NoResize |
                                  ImGuiWindowFlags_NoMove |
                                  ImGuiWindowFlags_NoBringToFrontOnFocus |
                                  ImGuiWindowFlags_NoTitleBar;
        // ImGui::SetNextWindowPos(ImVec2(width / 1.5, 0));
        ImGui::SetNextWindowSize(
            ImVec2(width - (width / 1.5), height));
        // ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);

        ImGui::End();
    };

    window.Run([&](Shader &framebuffer_shader) {
        static double lastTime = 0;
        double now = glfwGetTime();
        timestep = now - lastTime;
        lastTime = now;

        // clang-format off
    }, Function_GUI, [&]() {}, [&](Shader &) {}, [&](Framebuffer::ColorAttachement &) {});
    // clang-format on

    return 0;
}