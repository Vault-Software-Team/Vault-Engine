using System;
using System.Runtime.CompilerServices;

namespace Vault
{
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
        public static string ToString(Vector2 vec)
        {
            return InternalCalls.float_ToString(vec.x) + " " + InternalCalls.float_ToString(vec.y);
        }
        public static string ToString(Vector3 vec)
        {
            return InternalCalls.float_ToString(vec.x) + " " + InternalCalls.float_ToString(vec.y) + " " + InternalCalls.float_ToString(vec.z);
        }
        public static string ToString(Vector4 vec)
        {
            return InternalCalls.float_ToString(vec.x) + " " + InternalCalls.float_ToString(vec.y) + " " + InternalCalls.float_ToString(vec.z) + " " + InternalCalls.float_ToString(vec.w);
        }

        public static Vector2 StringToVector2(string value)
        {
            string[] split = value.Split(" ");
            return new Vector2(float.Parse(split[0]), float.Parse(split[1]));
        }

        public static Vector3 StringToVector3(string value)
        {
            string[] split = value.Split(" ");
            return new Vector3(float.Parse(split[0]), float.Parse(split[1]), float.Parse(split[2]));
        }


        public static Vector4 StringToVector4(string value)
        {
            string[] split = value.Split(" ");
            return new Vector4(float.Parse(split[0]), float.Parse(split[1]), float.Parse(split[2]), float.Parse(split[3]));
        }

    }

    public class Scene
    {
        public static void LoadScene(string path)
        {
            InternalCalls.Scene_LoadScene(path);
        }

    }


    public class Entity
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static object cpp_GetClassInstance(string id, string type);
    
        public string ID, name, tag;
        public void VAULT_ENGINE_ScriptInstanceInit(string ID) {
            SetObjectID(ID);
        }
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

        public static Entity GetByID(string id)
        {
            Entity e = new()
            {
                ID = id,
                name = InternalCalls.GameObject_GetName(id),
                tag = InternalCalls.GameObject_GetTag(id),
            };
            e.transform = e.GetComponent<Transform>();

            return e;
        }

        protected virtual void OnCollisionEnter2D(string ID) { }

        protected virtual void OnCollisionExit2D(string ID) { }

        public T GetComponent<T>() where T : Component, new()
        {
            T component = new T() { Entity = this };
            return (T)(object)component;

            return null;
        }
        
        public T? As<T>() where T : Entity, new()   
        {
           object instance = cpp_GetClassInstance(ID, typeof(T).Name);
           if (instance == null) return null;
           return instance as T;
        }
    }
}