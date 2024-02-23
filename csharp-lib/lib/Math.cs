using System;
using System.Runtime.CompilerServices;

namespace Vault
{
    public class Math
    {
        public static readonly float pi = 3.14159265359f;

        public static float ToDegrees(float radians)
        {
            return radians * (180 / pi);
        }
        public static float ToRadians(float degrees)
        {
            return degrees * (pi / 180);
        }

        public static Vector3 ToDegrees(Vector3 radians)
        {
            return radians * (180 / pi);
        }

        public static Vector3 ToRadians(Vector3 degrees)
        {
            return degrees * (pi / 180);
        }
    }
}