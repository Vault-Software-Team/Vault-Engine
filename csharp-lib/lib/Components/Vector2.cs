using System;
using System.Runtime.CompilerServices;

namespace Vault
{
    public class Vector2 {
        public float x,y;

        public Vector2(float x, float y) {
            this.x = x;
            this.y = y;
        }

        public static Vector2 zero = new Vector2(0, 0);


        public static Vector2 operator +(Vector2 a, Vector2 b)
        {
            return new Vector2(a.x + a.x, a.y + a.y);
        }

        public static Vector2 operator -(Vector2 a, Vector2 b)
        {
            return new Vector2(a.x - a.x, a.y - a.y);
        }

        public static Vector2 operator /(Vector2 a, Vector2 b)
        {
            return new Vector2(a.x / a.x, a.y / a.y);
        }

        public static Vector2 operator *(Vector2 a, Vector2 b)
        {
            return new Vector2(a.x * a.x, a.y * a.y);
        }
    }
}