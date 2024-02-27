#pragma once

#include <string>
namespace Engine {
    namespace Audio2D {
        void PlaySound(const std::string &path, float volume = 1.0f, bool loop = false, int channel = -1);
        void StopSound(int channel = -1);
        void PlayMusic(const std::string &path, float volume = 1.0f, bool loop = false);
        void StopMusic();
    } // namespace Audio2D
} // namespace Engine