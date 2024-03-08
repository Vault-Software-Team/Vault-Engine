using System.Runtime.CompilerServices;


namespace Vault
{
    public class Mathf
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static float Deg2Rad(float value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static float Rad2Deg(float value);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static float Abs(float value);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static float Acos(float value);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static float Asin(float value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static float Atan(float value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static float Atan2(float x, float y);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static float Ceil(float value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static float Clamp(float value, float min, float max);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static float Cos(float value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static float Sin(float value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static float Sqrt(float value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static float Tan(float value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static float Round(float value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static float Pow(float x, float y);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static float Log(float value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static float Log10(float value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static float Max(float x, float y);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static float Min(float x, float y);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static float Exp(float x);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static float Lerp(float from, float to, float lerp_speed);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static int RandomRange(int min, int max);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static float FloatRandomRange(float min, float max);


        public static readonly float zero = 0;
        public static readonly float PI = 3.141592653589793f;
        public static float Infinity
        {
            get { return 1 / zero; }
            set { }
        }

        public static float NegativeInfinity
        {
            get { return -1 / zero; }
            set { }
        }
    }
}
