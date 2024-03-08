using System;
using System.Runtime.CompilerServices;
using Vault;

namespace Vault
{
    public class Rigidbody2D : Component
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void Rigidbody2D_GetKey(string key, string id, out string result);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void Rigidbody2D_SetType(int type, string id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern private static void Rigidbody2D_SetVelocity(float x, float y, string id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern private static void Rigidbody2D_SetAngularVelocity(float velocity, string id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern private static void Rigidbody2D_SetPosition(float x, float y, string id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern private static void Rigidbody2D_Force(float x, float y, string id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern private static void Rigidbody2D_Torque(float torque, string id);

        public void SetVelocity(float x, float y)
        {
            Rigidbody2D_SetVelocity(x, y, Entity.ID);
        }

        public void SetAngularVelocity(float velocity)
        {
            Rigidbody2D_SetAngularVelocity(velocity, Entity.ID);
        }

        public void SetPosition(float x, float y)
        {
            Rigidbody2D_SetPosition(x, y, Entity.ID);
        }

        public void Force(float x, float y)
        {
            Rigidbody2D_Force(x, y, Entity.ID);
        }

        public void Torque(float torque)
        {
            Rigidbody2D_Torque(torque, Entity.ID);
        }


        public Vector2 velocity
        {
            get
            {
                Rigidbody2D_GetKey("velocity", Entity.ID, out string result);
                return Format.StringToVector2(result);
            }
            set
            {
                Rigidbody2D_SetVelocity(value.x, value.y, Entity.ID);
            }
        }

        public int type
        {
            get
            {
                Rigidbody2D_GetKey("type", Entity.ID, out string result);
                return int.Parse(result);
            }
            set
            {
                Rigidbody2D_SetType(value, Entity.ID);
            }
        }
    }
}