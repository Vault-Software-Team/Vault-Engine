#include <Renderer/Stats.hpp>
#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
    #define GL_GLEXT_PROTOTYPES
    #define EGL_EGLEXT_PROTOTYPES
#else
    #include <glad/glad.h>
#endif
#include <GLFW/glfw3.h>

namespace VaultRenderer {
    DLL_API int Statistics::draw_calls;
    DLL_API std::string Statistics::vendor;
    DLL_API std::string Statistics::renderer;
    DLL_API std::string Statistics::version;
    DLL_API std::string Statistics::shading_language;
    DLL_API int Statistics::fps;
    DLL_API int Statistics::fps_count;

    void Statistics::SetStats() {
        vendor = (char *)glGetString(GL_VENDOR);
        renderer = (char *)glGetString(GL_RENDERER);
        version = (char *)glGetString(GL_VERSION);
        shading_language = (char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
        draw_calls = 0;
    }

    void Statistics::ResetDrawCalls() {
        draw_calls = 0;
    }

    void Statistics::DrawCall() {
        draw_calls++;
    }

    int Statistics::GetDrawCalls() {
        return draw_calls;
    }

    void Statistics::CalculateFPS(double &previous_time) {
        double current_time = glfwGetTime();
        fps_count++;

        if (current_time - previous_time >= 1.0) {
            fps = fps_count;
            previous_time = current_time;
            fps_count = 0;
        }
    }
} // namespace VaultRenderer