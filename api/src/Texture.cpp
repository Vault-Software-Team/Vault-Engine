#include <Renderer/Texture.hpp>
#include <stb_image/stb_image.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace VaultRenderer {
    Texture::~Texture() {
        if (texture_data.use_count() <= 1) {
            for (int i = 0; i < textures.size(); i++) {
                if (textures[i]->texture_filepath == texture_data->texture_filepath) {
                    textures.erase(textures.begin() + i);
                    break;
                }
            }
        }
    }

    Texture::Texture(const std::string &texture_file) {
        bool found = false;
        for (auto tex : textures) {
            if (tex->texture_filepath == texture_file) {
                texture_data = tex;
                found = true;
                break;
            }
        }

        if (!found) {
            texture_data = std::make_shared<t_texture>();
            texture_data->texture_filepath = texture_file;

            int width, height, nrChannels;
            stbi_set_flip_vertically_on_load(true);
            unsigned char *data = stbi_load(texture_data->texture_filepath.c_str(), &width, &height, &nrChannels, 0);

            glGenTextures(1, &texture_data->ID);
            glBindTexture(GL_TEXTURE_2D, texture_data->ID);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            if (nrChannels >= 4)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            else if (nrChannels == 3)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            else if (nrChannels == 1)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);

            glGenerateMipmap(GL_TEXTURE_2D);

            stbi_image_free(data);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }

    void Texture::Bind(uint32_t slot) {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, texture_data->ID);
    }
} // namespace VaultRenderer