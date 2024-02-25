#pragma once

#include "glm/fwd.hpp"
#include <mono/metadata/assembly.h>
#include <mono/jit/jit.h>

namespace Engine {
    namespace CSharpInternalFunctions {
        MonoString *SpriteRenderer_GetTexture(MonoString *ms_ID);
        MonoString *SpriteRenderer_GetColor(MonoString *ms_ID);
        void SpriteRenderer_SetTexture(MonoString *ms_ID, MonoString *ms_path);
        void SpriteRenderer_SetColor(MonoString *ms_ID, float r, float g, float b);
    } // namespace CSharpInternalFunctions
} // namespace Engine