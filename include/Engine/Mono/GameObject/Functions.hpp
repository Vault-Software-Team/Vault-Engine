#pragma once

#include "glm/fwd.hpp"
#include <mono/metadata/assembly.h>
#include <mono/jit/jit.h>

namespace Engine {
    namespace CSharpInternalFunctions {
        MonoString *GameObject_GetName(MonoString *ID);
        MonoString *GameObject_GetTag(MonoString *ID);
        MonoString *GameObject_GetIDByName(MonoString *name);
        MonoString *GameObject_GetIDByTag(MonoString *tag);
        MonoString *GameObject_InstantiatePrefab(MonoString *ms_path, MonoString *ms_name, MonoString *ms_tag, MonoString *ms_id);
        // have to do this fucked up traumatizing shenanigan and fuckery cause idk why Vector3 (C#) doesn't convert to glm::vec3
        MonoString *GameObject_InstantiatePrefabWithProps(MonoString *ms_path, float px, float py, float pz, float rx, float ry, float rz, float sx, float sy, float sz, MonoString *ms_name, MonoString *ms_tag, MonoString *ms_id);
        void Scene_LoadScene(MonoString *path);
    } // namespace CSharpInternalFunctions
} // namespace Engine