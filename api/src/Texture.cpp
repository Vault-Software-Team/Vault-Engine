#include <Renderer/Texture.hpp>
#include <stb_image/stb_image.h>
#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
    #define GL_GLEXT_PROTOTYPES
    #define EGL_EGLEXT_PROTOTYPES
#else
    #include <glad/glad.h>
#endif
#include <GLFW/glfw3.h>
#include <iostream>

namespace VaultRenderer {
    DLL_API std::vector<std::shared_ptr<Texture::t_texture>> Texture::textures;

    Texture::t_texture::~t_texture() {
        std::cout << "t_texture" << texture_filepath << " deleted\n";
        glDeleteTextures(1, &ID);
    }

    Texture::~Texture() {
        if (texture_data.use_count() <= 2) {
            for (int i = 0; i < textures.size(); i++) {
                if (textures[i]->texture_filepath == texture_data->texture_filepath) {
                    textures[i].reset();
                    textures.erase(textures.begin() + i);

                    break;
                }
            }
        }
    }

    Texture::Texture(const std::string &texture_file, const TextureType &type) {
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
            texture_data->type = type;

            if (type == TEXTURE_HDRI) {
                stbi_set_flip_vertically_on_load(true);
                int width, height, nrComponents;
                float *data = stbi_loadf(texture_data->texture_filepath.c_str(), &width, &height, &nrComponents, 0);

                if (data) {
                    glGenTextures(1, &texture_data->ID);
                    glBindTexture(GL_TEXTURE_2D, texture_data->ID);
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                    stbi_image_free(data);
                } else {
                    std::cout << "Failed to load HDR image." << std::endl;
                }
            } else {

                int width, height, nrChannels;
                stbi_set_flip_vertically_on_load(true);
                unsigned char *data = stbi_load(texture_data->texture_filepath.c_str(), &width, &height, &nrChannels, 0);

                if (nrChannels >= 3) glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                glGenTextures(1, &texture_data->ID);
                glBindTexture(GL_TEXTURE_2D, texture_data->ID);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

                switch (type) {
                case TEXTURE_NORMAL:
                    if (nrChannels >= 4)
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                    else if (nrChannels == 3)
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                    else if (nrChannels == 1)
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
                    break;
                case TEXTURE_HEIGHT:
                    // glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                    if (nrChannels >= 4)
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                    else if (nrChannels == 3)
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                    else if (nrChannels == 1)
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
                    break;
                default:
                    if (nrChannels >= 4)
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                    else if (nrChannels == 3)
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                    else if (nrChannels == 1)
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
                    break;
                }

                // glGenerateMipmap(GL_TEXTURE_2D);

                stbi_image_free(data);
                glBindTexture(GL_TEXTURE_2D, 0);

                textures.push_back(texture_data);
            }
        }
    }

    void Texture::Bind(uint32_t slot) {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, texture_data->ID);
    }
} // namespace VaultRenderer