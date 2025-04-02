#pragma once

#include "glm/fwd.hpp"
#include <mono/metadata/assembly.h>
#include <mono/jit/jit.h>

namespace Engine {
    namespace CSharpInternalFunctions {
        MonoString *SpritesheetAnimator_GetTexture(MonoString *ms_ID);
        MonoString *SpritesheetAnimator_GetColor(MonoString *ms_ID);
        MonoString *SpritesheetAnimator_GetCurrentAnimation(MonoString *ms_ID);
        bool SpritesheetAnimator_GetAnimate(MonoString *ms_ID);
        void SpritesheetAnimator_SetTexture(MonoString *ms_ID, MonoString *ms_path);
        void SpritesheetAnimator_SetColor(MonoString *ms_ID, float r, float g, float b);
        void SpritesheetAnimator_SetCurrentAnimation(MonoString *ms_ID, MonoString *ms_current_animation);
        void SpritesheetAnimator_SetAnimate(MonoString *ms_ID, bool ms_animate);
    } // namespace CSharpInternalFunctions
} // namespace Engine