using System;
using System.Runtime.CompilerServices;

namespace Vault
{
    internal static class InternalCalls
    {
        // [MethodImpl(MethodImplOptions.InternalCall)]
        // public static extern Vector3 GetPosition();
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern string GameObject_GetName(string ID);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern string GameObject_GetTag(string ID);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern string GameObject_GetIDByName(string name);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern string GameObject_GetIDByTag(string tag);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Scene_LoadScene(string path);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern string GameObject_InstantiatePrefab(string path, string name = "__VAULT_DEFAULT_NAME__", string tag = "__VAULT_DEFAULT_TAG__", string id = "DEFAULT");
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern string GameObject_InstantiatePrefabWithProps(string path, float px, float py, float pz, float rx, float ry, float rz, float sx, float sy, float sz, string name = "__VAULT_DEFAULT_NAME__", string tag = "__VAULT_DEFAULT_TAG__", string id = "DEFAULT");
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Transform_GetPosition(string ID, out string result);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Transform_GetRotation(string ID, out string result);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Transform_GetScale(string ID, out string result);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Transform_SetField(string ID, string key, float x, float y, float z);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Audio2D_PlaySound(string ms_path, float volume, bool loop, int channel);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Audio2D_StopSound(int channel);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Audio2D_PlayMusic(string ms_path, float volume, bool loop);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Audio2D_StopMusic();

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern string float_ToString(float f);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern string double_ToString(double f);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern string int_ToString(int f);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern float Time_GetDeltaTime();
    }

}