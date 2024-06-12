#pragma once
#include <dllapi.hpp>
#include <map>
#include <memory>
#include "Shader.hpp"
#include <freetype/ft2build.h>
#include FT_FREETYPE_H

namespace VaultRenderer {

    class DLL_API Font {
    public:
        static DLL_API FT_Library ft;
        static DLL_API std::unique_ptr<Shader> font_shader;
        static DLL_API int InitFT();

        uint32_t VAO, VBO;
        std::string font_path;

        struct DLL_API Character {
            uint32_t TextureID; // ID handle of the glyph texture
            glm::ivec2 Size;    // Size of glyph
            glm::ivec2 Bearing; // Offset from baseline to left/top of glyph
            uint32_t Advance;   // Offset to advance to next glyph
        };
        std::map<char, Character> Characters;

        Font(const char *font, uint32_t scale);
        void Draw(Shader &shader, const glm::mat4 &model, const std::string &text, glm::vec3 color, glm::vec3 bloomColor, float x, float y, float scale, float y_offset, uint32_t entity);
    };
} // namespace VaultRenderer
