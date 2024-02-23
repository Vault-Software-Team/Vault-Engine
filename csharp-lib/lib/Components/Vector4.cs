using System;
using System.Runtime.CompilerServices;

namespace Vault
{
    public class Vector4
    {
        public float x, y, z, w;

        public Vector4(float X, float Y, float Z, float W)
        {
            x = X;
            y = Y;
            z = Z;
            w = W;
        }

        public static Vector4 zero = new Vector4(0, 0, 0, 0);


        public static Vector4 operator +(Vector4 a, Vector4 b)
        {
            return new Vector4(a.x + a.x, a.y + a.y, a.z + b.z, a.w + b.w);
        }

        public static Vector4 operator -(Vector4 a, Vector4 b)
        {
            return new Vector4(a.x - a.x, a.y - a.y, a.z - b.z, a.w -= b.w);
        }

        public static Vector4 operator /(Vector4 a, Vector4 b)
        {
            return new Vector4(a.x / a.x, a.y / a.y, a.z / b.z, a.w / b.w);
        }

        public static Vector4 operator *(Vector4 a, Vector4 b)
        {
            return new Vector4(a.x * a.x, a.y * a.y, a.z * b.z, a.w * b.w);
        }
    }
}