#include <SDL2/SDL_mixer.h>
#include <Engine/Audio.hpp>
#include <iostream>

namespace Engine {
    void Audio2D::PlaySound(const std::string &path, float volume, bool loop, int channel) {
        Mix_Chunk *chunk = Mix_LoadWAV(path.c_str());

        Mix_VolumeChunk(chunk, volume * 128);
        Mix_PlayChannel(channel, chunk, loop ? -1 : 0);
    }

    void Audio2D::StopSound(int channel) {
        Mix_HaltChannel(channel);
    }

    void Audio2D::PlayMusic(const std::string &path, float volume, bool loop) {
        Mix_Music *music = Mix_LoadMUS(path.c_str());
        if (music == NULL) {
            std::cout << "Failed to load music: " + path << "\n";
            return;
        }

        Mix_VolumeMusic(volume * 128);
        Mix_PlayMusic(music, loop ? -1 : 0);
    }

    void Audio2D::StopMusic() {
        Mix_HaltMusic();
    }

    void Audio2D::InitAudio() {
        Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    }
} // namespace Engine