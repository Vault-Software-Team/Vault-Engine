#pragma once
#include <dllapi.hpp>
#include <memory>
#include <string>
#include <vector>

namespace VaultRenderer {
    enum TextureType {
        TEXTURE_DIFFUSE,
        TEXTURE_SPECULAR,
        TEXTURE_NORMAL,
        TEXTURE_HEIGHT,
    };
    class DLL_API Texture {
    public:
        struct t_texture {
            uint32_t ID;
            std::string texture_filepath;
            TextureType type;

            t_texture() = default;
            ~t_texture();
        };

        static DLL_API std::vector<std::shared_ptr<t_texture>> textures;

        std::shared_ptr<t_texture> texture_data;

        Texture(const std::string &texture_file, const TextureType &type = TEXTURE_DIFFUSE);
        ~Texture();

        void Bind(uint32_t slot);
    };
} // namespace VaultRenderer
