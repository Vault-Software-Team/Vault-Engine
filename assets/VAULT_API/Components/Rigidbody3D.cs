using System;
using System.Runtime.CompilerServices;
using Vault;

namespace Vault
{
    public class Rigidbody3D : Component
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void Rigidbody3D_GetKey(string key, string id, out string result);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void Rigidbody3D_SetKey(string key, string id, string value);

        // Component Functions
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void Rigidbody3D_AddForce(string id, float x, float y, float z);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void Rigidbody3D_AddTorque(string id, float x, float y, float z);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void Rigidbody3D_AddForceAtPosition(string id, float fx, float fy, float fz, float px, float py, float pz);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void Rigidbody3D_SetVelocity(string id, float x, float y, float z);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void Rigidbody3D_SetAngularVelocity(string id, float x, float y, float z);

        void AddForce(Vector3 force)
        {
            Rigidbody3D_AddForce(Entity.ID, force.x, force.y, force.z);
        }

        void AddTorque(Vector3 torque)
        {
            Rigidbody3D_AddTorque(Entity.ID, torque.x, torque.y, torque.z);
        }

        void AddForceAtPosition(Vector3 force, Vector3 position)
        {
            Rigidbody3D_AddForceAtPosition(Entity.ID, force.x, force.y, force.z, position.x, position.y, position.z);
        }

        void SetVelocity(Vector3 velocity)
        {
            Rigidbody3D_SetVelocity(Entity.ID, velocity.x, velocity.y, velocity.z);
        }

        void SetAngularVelocity(Vector3 velocity)
        {
            Rigidbody3D_SetAngularVelocity(Entity.ID, velocity.x, velocity.y, velocity.z);
        }

        public float mass
        {
            get
            {
                Rigidbody3D_GetKey("mass", Entity.ID, out string result);
                return float.Parse(result);
            }
            set
            {
                Rigidbody3D_SetKey("mass", Entity.ID, Format.ToString(value));
            }
        }

        public float restitution
        {
            get
            {
                Rigidbody3D_GetKey("restitution", Entity.ID, out string result);
                return float.Parse(result);
            }
            set
            {
                Rigidbody3D_SetKey("restitution", Entity.ID, Format.ToString(value));
            }
        }

        public float friction
        {
            get
            {
                Rigidbody3D_GetKey("friction", Entity.ID, out string result);
                return float.Parse(result);
            }
            set
            {
                Rigidbody3D_SetKey("friction", Entity.ID, Format.ToString(value));
            }
        }

        public bool trigger
        {
            get
            {
                Rigidbody3D_GetKey("trigger", Entity.ID, out string result);
                return result == "true";
            }
            set
            {
                Rigidbody3D_SetKey("trigger", Entity.ID, value ? "true" : "false");
            }
        }

        public bool fixedRotation
        {
            get
            {
                Rigidbody3D_GetKey("fixedRotation", Entity.ID, out string result);
                return result == "true";
            }
            set
            {
                Rigidbody3D_SetKey("fixedRotation", Entity.ID, value ? "true" : "false");
            }
        }

        public Vector3 velocity
        {
            get
            {
                Rigidbody3D_GetKey("vecloity", Entity.ID, out string result);
                string[] split = result.Split(" ");
                return new Vector3(float.Parse(split[0]), float.Parse(split[1]), float.Parse(split[2]));
            }
            set
            {
                Rigidbody3D_SetVelocity(Entity.ID, value.x, value.y, value.z);
            }
        }

        public Vector3 angularVelocity
        {
            get
            {
                Rigidbody3D_GetKey("angularVecloity", Entity.ID, out string result);
                string[] split = result.Split(" ");
                return new Vector3(float.Parse(split[0]), float.Parse(split[1]), float.Parse(split[2]));
            }
            set
            {
                Rigidbody3D_SetAngularVelocity(Entity.ID, value.x, value.y, value.z);
            }
        }
    }
}