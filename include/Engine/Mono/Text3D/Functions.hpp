#pragma once

#include "glm/fwd.hpp"
#include <mono/metadata/assembly.h>
#include <mono/jit/jit.h>

namespace Engine {
    namespace CSharpInternalFunctions {
        MonoString *Text3D_GetText(MonoString *ms_ID);
        float Text3D_GetScale(MonoString *ms_ID);
        float Text3D_GetYOffset(MonoString *ms_ID);
        MonoString *Text3D_GetColor(MonoString *ms_ID);

        void Text3D_SetText(MonoString *ms_ID, MonoString *ms_content);
        void Text3D_SetScale(MonoString *ms_ID, float val);
        void Text3D_SetYOffset(MonoString *ms_ID, float val);
        void Text3D_SetColor(MonoString *ms_ID, float r, float g, float b);

    } // namespace CSharpInternalFunctions
} // namespace Engine