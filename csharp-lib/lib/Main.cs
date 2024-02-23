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
        internal static extern void Transform_GetPosition(string ID, out string result);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Transform_GetRotation(string ID, out string result);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Transform_GetScale(string ID, out string result);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Transform_SetField(string ID, string key, float x, float y, float z);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern string float_ToString(float f);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern string double_ToString(double f);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern string int_ToString(int f);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern float Time_GetDeltaTime();
    }

    public class Format
    {
        public static string ToString(float value)
        {
            return InternalCalls.float_ToString(value);
        }

        public static string ToString(double value)
        {
            return InternalCalls.double_ToString(value);
        }

        public static string ToString(int value)
        {
            return InternalCalls.int_ToString(value);
        }
    }

    public class Entity
    {
        public string ID, name, tag;
        protected virtual void OnStart(string ID) { }
        protected virtual void OnUpdate() { }

        public Transform transform;

        protected void SetObjectID(string ID)
        {
            this.ID = ID;
            name = InternalCalls.GameObject_GetName(ID);
            tag = InternalCalls.GameObject_GetTag(ID);
            transform = GetComponent<Transform>();
        }

        public T GetComponent<T>() where T : Component, new()
        {
            T component = new T() { Entity = this };
            return (T)(object)component;

            return null;
        }
    }
}