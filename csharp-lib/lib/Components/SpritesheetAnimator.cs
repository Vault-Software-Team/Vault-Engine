using System;
using System.Dynamic;
using System.Runtime.CompilerServices;

namespace Vault
{
    public class SpritesheetRenderer : Component
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string SpritesheetRenderer_GetTexture(string ID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SpritesheetRenderer_SetTexture(string ID, string path);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string SpritesheetRenderer_GetColor(string ID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SpritesheetRenderer_SetColor(string ID, float r, float g, float b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string SpritesheetRenderer_GetCurrentAnimation(string ID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SpritesheetRenderer_SetCurrentAnimation(string ID, string value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool SpritesheetRenderer_GetAnimate(string ID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SpritesheetRenderer_SetAnimate(string ID, bool value);

        public string texture
        {
            get
            {
                return SpritesheetRenderer_GetTexture(Entity.ID);
            }
            set
            {
                SpritesheetRenderer_SetTexture(Entity.ID, value);
            }
        }

        public Vector3 color
        {
            get
            {
                return Format.StringToVector3(SpritesheetRenderer_GetColor(Entity.ID));
            }
            set
            {
                SpritesheetRenderer_SetColor(Entity.ID, value.x, value.y, value.z);
            }
        }

        public string currentAnimation
        {
            get
            {
                return SpritesheetRenderer_GetCurrentAnimation(Entity.ID);
            }
            set
            {
                SpritesheetRenderer_SetCurrentAnimation(Entity.ID, value);
            }
        }

        public bool animate
        {
            get
            {
                return SpritesheetRenderer_GetAnimate(Entity.ID);
            }
            set
            {
                SpritesheetRenderer_SetAnimate(Entity.ID, value);
            }
        }
    }
}