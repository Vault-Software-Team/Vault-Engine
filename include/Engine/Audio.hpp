#pragma once

#include <string>
namespace Engine {
    class Audio2D {
    public:
        static void PlaySound(const std::string &path, float volume = 1.0f, bool loop = false, int channel = -1);
        static void StopSound(int channel = -1);
        static void PlayMusic(const std::string &path, float volume = 1.0f, bool loop = false);
        static void StopMusic();
        static void InitAudio();
    };
} // namespace Engine