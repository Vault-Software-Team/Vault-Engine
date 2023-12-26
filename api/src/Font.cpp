#include <Renderer/Font.hpp>
#include <iostream>
#include <Renderer/Mesh.hpp>
#include <Renderer/Stats.hpp>

namespace VaultRenderer {
    DLL_API FT_Library ft;
    DLL_API Shader *font_shader;

    int Font::InitFT() {
        font_shader = std::make_unique<Shader>("../shaders/text_shader.glsl");
        if (FT_Init_FreeType(&ft)) {
            std::cout << "ERROR: Couldn't init FreeType Library\n";
            return -1;
        }
        return 0;
    }

    Font::Font(const char *font, uint32_t scale) : font_path(font) {
        FT_Face face;
        if (FT_New_Face(ft, font, 0, &face)) {
            std::cout << "ERROR: Failed to load font " + std::string(font) << "\n";
        }

        FT_Set_Pixel_Sizes(face, 0, scale);

        if (FT_Load_Char(face, 'X', FT_LOAD_RENDER)) {
            std::cout << "ERROR: Failed to load Glyph\n";
        }

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        for (unsigned char c = 0; c < 128; c++) {
            if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
                std::cout << "ERROR: Failed to load Glyph\n";
                continue;
            }
            // generate texture
            uint32_t texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                (uint32_t)face->glyph->advance.x};
            Characters.insert(std::pair<char, Character>(c, character));
        }

        FT_Done_Face(face);

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 6, NULL, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texUV));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void Font::Draw(Shader &shader, const glm::mat4 &model, const std::string &text, glm::vec3 color, glm::vec3 bloomColor, float x, float y, float scale, float y_offset) {
        float saved_x = x, saved_y = y;
        glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);

        shader.Bind();
        shader.SetUniformMat4("transformModel", model);
        shader.SetUniform3f("bloomColor", bloomColor.x, bloomColor.y, bloomColor.z);
        shader.SetUniform3f("textColor", color.x, color.y, color.z);
        shader.SetUniform1i("texture_diffuse.tex", 0);
        shader.SetUniform1i("texture_diffuse.defined", 1);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(VAO);

        // iterate through all characters
        std::string::const_iterator c;
        float newLine;
        for (c = text.begin(); c != text.end(); c++) {
            Character ch = Characters[*c];

            float xpos = x + ch.Bearing.x * scale;
            float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
            ypos += newLine;
            if (*c == '\n') {
                x = saved_x;
                newLine -= (ch.Bearing.y * scale) + y_offset;
                continue;
            }

            float w = ch.Size.x * scale;
            float h = ch.Size.y * scale;

            std::vector<Vertex> vertices = {
                {glm::vec3(xpos, ypos + h, 0), glm::vec2(0.0f, 0.0f)},
                {glm::vec3(xpos, ypos, 0), glm::vec2(0.0f, 1.0f)},
                {glm::vec3(xpos + w, ypos, 0), glm::vec2(1.0f, 1.0f)},

                {glm::vec3(xpos, ypos + h, 0), glm::vec2(0.0f, 0.0f)},
                {glm::vec3(xpos + w, ypos, 0), glm::vec2(1.0f, 1.0f)},
                {glm::vec3(xpos + w, ypos + h, 0), glm::vec2(1.0f, 0.0f)}};

            glBindTexture(GL_TEXTURE_2D, ch.TextureID);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * vertices.size(), vertices.data());
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDepthFunc(GL_LESS);

            Statistics::DrawCall();
            glDrawArrays(GL_TRIANGLES, 0, vertices.size());

            x += (ch.Advance >> 6) * scale;
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glDepthFunc(GL_LEQUAL);
    }
} // namespace VaultRenderer