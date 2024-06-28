using System;
using System.Runtime.CompilerServices;
using Vault;

namespace Vault
{
    public class PointLight : Component
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static string PointLight_GetKey(string key, string id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void PointLight_SetKey(string key, string id, string value);

        public float intensity
        {
            get
            {
                return float.Parse(PointLight_GetKey("intensity", Entity.ID));
            }
            set
            {
                PointLight_SetKey("intensity", Entity.ID, Format.ToString(value));
            }
        }

        public float shadowFarPlane
        {
            get
            {
                return float.Parse(PointLight_GetKey("shadowFarPlane", Entity.ID));
            }
            set
            {
                PointLight_SetKey("shadowFarPlane", Entity.ID, Format.ToString(value));
            }
        }

        public bool enableShadowMapping
        {
            get
            {
                return PointLight_GetKey("enableShadowMapping", Entity.ID) == "true";
            }
            set
            {
                PointLight_SetKey("enableShadowMapping", Entity.ID, value ? "true" : "false");
            }
        }

        public Vector3 color
        {
            get
            {
                return Format.StringToVector3(PointLight_GetKey("color", Entity.ID));
            }
            set
            {
                PointLight_SetKey("color", Entity.ID, Format.ToString(value.x) + " " + Format.ToString(value.y) + " " + Format.ToString(value.z));
            }
        }
    }
}