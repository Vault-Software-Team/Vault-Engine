using System;
using System.Dynamic;
using System.Runtime.CompilerServices;

namespace Vault
{
    public class SpriteRenderer : Component
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string SpriteRenderer_GetTexture(string ID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SpriteRenderer_SetTexture(string ID, string path);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string SpriteRenderer_GetColor(string ID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SpriteRenderer_SetColor(string ID, float r, float g, float b);

        public string texture
        {
            get
            {
                return SpriteRenderer_GetTexture(Entity.ID);
            }
            set
            {
                SpriteRenderer_SetTexture(Entity.ID, value);
            }
        }

        public Vector3 color
        {
            get
            {
                return Format.StringToVector3(SpriteRenderer_GetColor(Entity.ID));
            }
            set
            {
                SpriteRenderer_SetColor(Entity.ID, value.x, value.y, value.z);
            }
        }
    }
}