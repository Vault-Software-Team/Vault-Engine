#include <Engine/Mono/Audio/Functions.hpp>
#include <Engine/Audio.hpp>
#include <Engine/Mono/HelperFunctions.hpp>

namespace Engine {
    namespace CSharpInternalFunctions {
        void Audio2D_PlaySound(MonoString *ms_path, float volume, bool loop, int channel) {
            Audio2D::PlaySound(CSharpHelper::MonoStrToString(ms_path), volume, loop, channel);
        }
        void Audio2D_StopSound(int channel) {
            Audio2D::StopSound(channel);
        }
        void Audio2D_PlayMusic(MonoString *ms_path, float volume, bool loop) {
            Audio2D::PlayMusic(CSharpHelper::MonoStrToString(ms_path), volume, loop);
        }
        void Audio2D_StopMusic() {
            Audio2D::StopMusic();
        }
    } // namespace CSharpInternalFunctions
} // namespace Engine