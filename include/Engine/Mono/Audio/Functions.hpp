#pragma once

#include "glm/fwd.hpp"
#include "mono/metadata/object.h"
#include <mono/metadata/assembly.h>
#include <mono/jit/jit.h>

namespace Engine {
    namespace CSharpInternalFunctions {
        void Audio2D_PlaySound(MonoString *ms_path, float volume, bool loop, int channel);
        void Audio2D_StopSound(int channel);
        void Audio2D_PlayMusic(MonoString *ms_path, float volume, bool loop);
        void Audio2D_StopMusic();
    } // namespace CSharpInternalFunctions
} // namespace Engine