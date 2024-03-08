using System;
using System.Runtime.CompilerServices;

namespace Vault
{
    public class Vector3
    {
        public float x, y, z;

        public Vector3(float x, float y, float z)
        {
            this.x = x;
            this.y = y;
            this.z = z;
        }

        public Vector3(Vector2 vector, float z)
        {
            this.x = vector.x;
            this.y = vector.y;
            this.z = z;
        }

        public Vector3(float x, Vector2 vector)
        {
            this.x = x;
            this.y = vector.x;
            this.z = vector.y;
        }

        public static Vector3 zero = new Vector3(0, 0, 0);


        public static Vector3 operator +(Vector3 a, Vector3 b)
        {
            return new Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
        }

        public static Vector3 operator -(Vector3 a, Vector3 b)
        {
            return new Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
        }

        public static Vector3 operator /(Vector3 a, Vector3 b)
        {
            return new Vector3(a.x / b.x, a.y / b.y, a.z / b.z);
        }

        public static Vector3 operator *(Vector3 a, Vector3 b)
        {
            return new Vector3(a.x * b.x, a.y * b.y, a.z * b.z);
        }

        public static Vector3 operator +(Vector3 a, float b)
        {
            return new Vector3(a.x + b, a.y + b, a.z + b);
        }

        public static Vector3 operator -(Vector3 a, float b)
        {
            return new Vector3(a.x - b, a.y - b, a.z - b);
        }

        public static Vector3 operator /(Vector3 a, float b)
        {
            return new Vector3(a.x / b, a.y / b, a.z / b);
        }

        public static Vector3 operator *(Vector3 a, float b)
        {
            return new Vector3(a.x * b, a.y * b, a.z * b);
        }

        public static explicit operator int(Vector3 v)
        {
            throw new NotImplementedException();
        }

        public string ToString()
        {
            return Format.ToString(x) + " " + Format.ToString(y) + " " + Format.ToString(z);
        }

        public TVector3 to
        {
            get
            {
                return new TVector3(x, y, z, "", "");
            }
            set { }
        }
    }
}