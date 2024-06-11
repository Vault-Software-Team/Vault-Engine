using System;
using System.Runtime.CompilerServices;
using Vault;

namespace Vault
{
    public class Camera : Component
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static float Camera_GetKey(string key, string id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void Camera_SetKey(string key, string id, float value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static string Camera_GetFront();


        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static bool Camera_GetBool(string key, string id);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void Camera_SetBool(string key, string id, bool value);

        public float FOV
        {
            get
            {
                return Camera_GetKey("FOV", Entity.ID);
            }
            set
            {
                Camera_SetKey("FOV", Entity.ID, value);
            }
        }

        public float near
        {
            get
            {
                return Camera_GetKey("near", Entity.ID);
            }
            set
            {
                Camera_SetKey("near", Entity.ID, value);
            }
        }

        public float far
        {
            get
            {
                return Camera_GetKey("far", Entity.ID);
            }
            set
            {
                Camera_SetKey("far", Entity.ID, value);
            }
        }

        public bool is2D
        {
            get
            {
                return Camera_GetBool("is2D", Entity.ID);
            }
            set
            {
                Camera_SetBool("is2D", Entity.ID, value);
            }
        }

        public bool depthCamera
        {
            get
            {
                return Camera_GetBool("depthCamera", Entity.ID);
            }
            set
            {
                Camera_SetBool("depthCamera", Entity.ID, value);
            }
        }

        public bool mainCamera
        {
            get
            {
                return Camera_GetBool("mainCamera", Entity.ID);
            }
            set
            {
                Camera_SetBool("mainCamera", Entity.ID, value);
            }
        }

        public Vector3 front
        {
            get
            {
                return Format.StringToVector3(Camera_GetFront());
            }
            private set { }
        }
    }
}