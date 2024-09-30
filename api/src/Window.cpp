#include "Editor/GUI/MainGUI.hpp"
#include "GLFW/glfw3.h"
#include "Renderer/Stats.hpp"
#include <Renderer/Window.hpp>
#include <iostream>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <icons/icons.h>
#include <Renderer/Framebuffer.hpp>
#include <Renderer/Bloom.hpp>
#include <ImGuizmo/ImGuizmo.h>
#include <stb_image/stb_image.h>

namespace VaultRenderer {
    DLL_API Window *Window::window;

    RendererConfig Window::Renderer;

    void Window::AspectRatioCameraViewport() {
        glViewport(0, 0, width, height);
    }
    void _AspectRatioCameraViewport() {
        float targetAspectRatio = (float)Window::window->targetWidth / (float)Window::window->targetHeight;

        int aspectWidth = VaultRenderer::Window::window->width;
        int aspectHeight = (int)((float)aspectWidth / targetAspectRatio);
        if (aspectHeight > VaultRenderer::Window::window->height) {
            aspectHeight = VaultRenderer::Window::window->height;
            aspectWidth = (int)((float)aspectHeight * targetAspectRatio);
        }
        int vpX = (int)(((float)VaultRenderer::Window::window->width / 2.0f) - ((float)aspectWidth / 2.0f));
        int vpY = (int)(((float)VaultRenderer::Window::window->height / 2.0f) - ((float)aspectHeight / 2.0f));

        glViewport(vpX, vpY, aspectWidth, aspectHeight);
    }

    void Window::SetMousePos(float x, float y) {
        mouse_pos.x = x;
        mouse_pos.y = y;
    }
    void Window::SetMousePos(glm::vec2 xy) {
        mouse_pos.x = xy.x;
        mouse_pos.y = xy.y;
    }

    Window::Window(const int width, const int height, const char *title, bool draw_screen) : width(width), height(height), title(title) {
        window = this;
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        glfw_window = glfwCreateWindow(width, height, title, NULL, NULL);
        if (glfw_window == NULL) {
            std::cout << "Failed to create GLFW Window\n";
            glfwTerminate();
            return;
        }

        glfwMakeContextCurrent(glfw_window);

        gladLoadGL();

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        // glEnable(GL_FRAMEBUFFER_SRGB);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glViewport(0, 0, width, height);

        // VSync V-Sync
        glfwSwapInterval(0);

        glfwSetFramebufferSizeCallback(glfw_window, FramebufferSizeCallback);

        // ImGui Setup
        SetupImGui();
        SetDefaultImGuiTheme();

        framebuffer = std::make_unique<Framebuffer>(true);
        m_PostProcessingFramebuffer = std::make_unique<Framebuffer>(false);

        // framebuffer->AddColorAttachement(1);
        framebuffer->draw_screen = draw_screen;
    }

    Window::~Window() {
        glfwTerminate();
    };

    void Window::Run(std::function<void(Shader &)> update_call, std::function<void()> gui_call, std::function<void()> shadow_render_call, std::function<void(Shader &)> framebuffer_shader_config, std::function<void(Framebuffer::ColorAttachement &)> mouse_picking) {
        static int before_width, before_height;
        Shader framebuffer_shader("./shaders/framebuffer.glsl");
        ImGuiIO &io = ImGui::GetIO();

        BloomRenderer bloomRenderer;
        bloomRenderer.Init(width, height);

        // Bloom Texture
        framebuffer->AddColorAttachement(GL_COLOR_ATTACHMENT1, GL_RGB16F, GL_RGB, GL_FLOAT);
        // Mouse Picking Texture
        framebuffer->AddColorAttachement(GL_COLOR_ATTACHMENT2, GL_R32I, GL_RED_INTEGER, GL_UNSIGNED_BYTE);

        double previous_time = glfwGetTime();

        while (!glfwWindowShouldClose(glfw_window)) {
            Statistics::CalculateFPS(previous_time);

            // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            if (!use_imgui_size)
                glfwGetWindowSize(glfw_window, &width, &height);

            bloomRenderer.mSrcViewportSize.x = width;
            bloomRenderer.mSrcViewportSize.x = height;

            bloomRenderer.mSrcViewportSizeFloat.x = width;
            bloomRenderer.mSrcViewportSizeFloat.x = height;

            // Framebuffer Shenanigans
            glfwPollEvents();

            shadow_render_call();
            // glViewport(0, 0, width, height);

            // glEnable(GL_FRAMEBUFFER_SRGB);
            framebuffer->Bind();
            VaultRenderer::Window::window->AspectRatioCameraViewport();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            update_call(framebuffer_shader);

            if (Renderer.Bloom) {
                glDisable(GL_BLEND);
                bloomRenderer.RenderBloomTexture(framebuffer->color_attachements[0].ID, 0.005f);
            }

            framebuffer_shader.Bind();
            framebuffer_shader_config(framebuffer_shader);
            framebuffer_shader.SetUniform1f("gamma", gamma);

            framebuffer->UnbindAndDrawOnScreen(framebuffer_shader);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, bloomRenderer.BloomTexture());
            framebuffer_shader.SetUniform1i("bloom_texture", 1);
            framebuffer_shader.SetUniform1i("useBloom", Renderer.Bloom);

            m_PostProcessingFramebuffer->Bind();
            framebuffer->DrawEverythingIntoAQuad(framebuffer_shader, framebuffer->texture);
            m_PostProcessingFramebuffer->Unbind();

            framebuffer->Bind();
            mouse_picking(framebuffer->color_attachements[1]);
            framebuffer->Unbind();

            /*
            Explaining this confusing ass code,

            Basically Framebuffer class has two framebuffers:
            - One where the elements get rendered it
            - And other where the framebuffer shader does stuff to the rendered texture
            So, to get the ACTUAL stuff you do framebuffer->framebuffer
            */

            // End of Framebuffer Shenanigans
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGuizmo::BeginFrame();

            // ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

            gui_call();
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            if (before_width != width || before_height != height) {
                framebuffer->width = width;
                framebuffer->height = height;
                m_PostProcessingFramebuffer->width = width;
                m_PostProcessingFramebuffer->height = height;

                framebuffer->RegenerateFramebuffer();
                m_PostProcessingFramebuffer->RegenerateFramebuffer();
                // Editor::GUI::framebufferTextureID = framebuffer->texture;
            }

            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
                GLFWwindow *backup_current_context = glfwGetCurrentContext();
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                glfwMakeContextCurrent(backup_current_context);
            }
            glfwSwapBuffers(glfw_window);
            before_width = width;
            before_height = height;
        }

        if (glfwWindowShouldClose(glfw_window)) {
            std::cout << "CLOSING FAGGOT\n";
            abort();
            std::cout << "aborted, hopefully";
        }
    }

    GLFWwindow *Window::GetGLFWWindow() const {
        return glfw_window;
    };

    void Window::SetClearColor(const float r, const float g, const float b) {
        glClearColor(pow(r, gamma), pow(gamma, g), pow(gamma, b), 1);
    }

    void Window::SetClearColor(const uint32_t hex_color) {
        glClearColor(pow(((hex_color >> 16) & 0xFF) / 255.0, gamma), pow(((hex_color >> 8) & 0xFF) / 255.0, gamma), pow(((hex_color) & 0xFF) / 255.0, gamma), 1);
    }

    void Window::SetViewport(const int width, const int height) {
        // glViewport(0, 0, width, height);
    }

    void Window::SetupImGui() {
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();

        io.ConfigWindowsMoveFromTitleBarOnly = true;
        // io.Fonts->AddFontDefault();
        ImGui::StyleColorsDark();

        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        io.FontDefault = io.Fonts->AddFontFromFileTTF(
            "./fonts/OpenSans-Semibold.ttf", 18.f);
        static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_16_FA,
                                               0};
        ImFontConfig icons_config;
        icons_config.MergeMode = true;
        icons_config.PixelSnapH = true;
        io.Fonts->AddFontFromFileTTF("./fonts/fa-solid-900.ttf", 16.0f, &icons_config, icons_ranges);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 2.0f);

        ImGui_ImplGlfw_InitForOpenGL(glfw_window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
    }

    void Window::SetDefaultImGuiTheme() {
        ImGuiStyle &style = ImGui::GetStyle();

        style.Alpha = 1.0f;
        style.DisabledAlpha = 0.6000000238418579f;
        style.WindowPadding = ImVec2(8.0f, 8.0f);
        style.WindowRounding = 7.0f;
        style.WindowBorderSize = 1.0f;
        style.WindowMinSize = ImVec2(32.0f, 32.0f);
        style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
        style.WindowMenuButtonPosition = ImGuiDir_Left;
        style.ChildRounding = 4.0f;
        style.ChildBorderSize = 1.0f;
        style.PopupRounding = 4.0f;
        style.PopupBorderSize = 1.0f;
        style.FramePadding = ImVec2(5.0f, 2.0f);
        style.FrameRounding = 3.0f;
        style.FrameBorderSize = 1.0f;
        style.ItemSpacing = ImVec2(6.0f, 6.0f);
        style.ItemInnerSpacing = ImVec2(6.0f, 6.0f);
        style.CellPadding = ImVec2(6.0f, 6.0f);
        style.IndentSpacing = 25.0f;
        style.ColumnsMinSpacing = 6.0f;
        style.ScrollbarSize = 15.0f;
        style.ScrollbarRounding = 9.0f;
        style.GrabMinSize = 10.0f;
        style.GrabRounding = 3.0f;
        style.TabRounding = 4.0f;
        style.TabBorderSize = 1.0f;
        style.TabMinWidthForCloseButton = 0.0f;
        style.ColorButtonPosition = ImGuiDir_Right;
        style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
        style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

        style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.4980392158031464f, 0.4980392158031464f, 0.4980392158031464f, 1.0f);
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.09803921729326248f, 0.09803921729326248f, 0.09803921729326248f, 1.0f);
        style.Colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        style.Colors[ImGuiCol_PopupBg] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.1882352977991104f, 0.9200000166893005f);
        style.Colors[ImGuiCol_Border] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.1882352977991104f, 0.2899999916553497f);
        style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 1);
        style.Colors[ImGuiCol_FrameBg] = ImVec4(0.0470588244497776f, 0.0470588244497776f, 0.0470588244497776f, 0.5400000214576721f);
        style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.1882352977991104f, 0.5400000214576721f);
        style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2274509817361832f, 1.0f);
        style.Colors[ImGuiCol_TitleBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
        style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.05882352963089943f, 0.05882352963089943f, 0.05882352963089943f, 1.0f);
        style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
        style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.1372549086809158f, 0.1372549086809158f, 0.1372549086809158f, 1.0f);
        style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.0470588244497776f, 0.0470588244497776f, 0.0470588244497776f, 0.5400000214576721f);
        style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.3372549116611481f, 0.3372549116611481f, 0.3372549116611481f, 0.5400000214576721f);
        style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.4000000059604645f, 0.4000000059604645f, 0.4000000059604645f, 0.5400000214576721f);
        style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.5568627715110779f, 0.5568627715110779f, 0.5568627715110779f, 0.5400000214576721f);
        style.Colors[ImGuiCol_CheckMark] = ImVec4(0.3294117748737335f, 0.6666666865348816f, 0.8588235378265381f, 1.0f);
        style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.3372549116611481f, 0.3372549116611481f, 0.3372549116611481f, 0.5400000214576721f);
        style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.5568627715110779f, 0.5568627715110779f, 0.5568627715110779f, 0.5400000214576721f);
        style.Colors[ImGuiCol_Button] = ImVec4(9.999999974752427e-07f, 9.999899930335232e-07f, 9.999899930335232e-07f, 1);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(1.0f, 0.2317596673965454f, 0.2317596673965454f, 0.5400000214576721f);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.3690987229347229f, 0.1093039140105247f, 0.1093039140105247f, 1.0f);
        style.Colors[ImGuiCol_Header] = ImVec4(0.0f, 0.0f, 0.0f, 0.5199999809265137f);
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.0f, 0.0f, 0.0f, 0.3600000143051147f);
        style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2274509817361832f, 0.3300000131130219f);
        style.Colors[ImGuiCol_Separator] = ImVec4(0.2784313857555389f, 0.2784313857555389f, 0.2784313857555389f, 0.2899999916553497f);
        style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.4392156898975372f, 0.4392156898975372f, 0.4392156898975372f, 0.2899999916553497f);
        style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.4000000059604645f, 0.4392156898975372f, 0.4666666686534882f, 1.0f);
        style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.2784313857555389f, 0.2784313857555389f, 0.2784313857555389f, 0.2899999916553497f);
        style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.4392156898975372f, 0.4392156898975372f, 0.4392156898975372f, 0.2899999916553497f);
        style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.4000000059604645f, 0.4392156898975372f, 0.4666666686534882f, 1.0f);
        style.Colors[ImGuiCol_Tab] = ImVec4(0.0f, 0.0f, 0.0f, 0.5199999809265137f);
        style.Colors[ImGuiCol_TabHovered] = ImVec4(0.1372549086809158f, 0.1372549086809158f, 0.1372549086809158f, 1.0f);
        style.Colors[ImGuiCol_TabActive] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2000000029802322f, 0.3600000143051147f);
        style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.0f, 0.0f, 0.0f, 0.5199999809265137f);
        style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.1372549086809158f, 0.1372549086809158f, 0.1372549086809158f, 1.0f);
        style.Colors[ImGuiCol_PlotLines] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
        style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
        style.Colors[ImGuiCol_PlotHistogram] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
        style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
        style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.5199999809265137f);
        style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.0f, 0.0f, 0.0f, 0.5199999809265137f);
        style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2784313857555389f, 0.2784313857555389f, 0.2784313857555389f, 0.2899999916553497f);
        style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
        style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2274509817361832f, 1.0f);
        style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 0.1f, 0.1f, 1.0f);
        style.Colors[ImGuiCol_NavHighlight] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
        style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 0.0f, 0.0f, 0.699999988079071f);
        style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.2000000029802322f);
        style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.3499999940395355f);
    }

    void Window::FramebufferSizeCallback(GLFWwindow *window, int width, int height) {
        // glViewport(0, 0, width, height);
    }

    void Window::SetWindowIcon(const std::string &path) {
        GLFWimage images[1];
        images[0].pixels = stbi_load(path.c_str(), &images[0].width, &images[0].height, 0, 4); // rgba channels
        glfwSetWindowIcon(GetGLFWWindow(), 1, images);
        stbi_image_free(images[0].pixels);
    }
} // namespace VaultRenderer