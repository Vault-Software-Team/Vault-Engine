using System;
using System.Runtime.CompilerServices;

namespace Vault
{
    public class Text3D : Component
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string Text3D_GetText(string ID);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Text3D_GetScale(string ID);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Text3D_GetYOffset(string ID);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string Text3D_GetColor(string ID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Text3D_SetText(string ID, string text);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Text3D_SetColor(string ID, float r, float g, float b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Text3D_SetScale(string ID, float val);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Text3D_SetYOffset(string ID, float val);

        public string text
        {
            get
            {
                return Text3D_GetText(Entity.ID);
            }
            set
            {
                Text3D_SetText(Entity.ID, value);
            }
        }
        public float scale
        {
            get
            {
                return Text3D_GetScale(Entity.ID);
            }
            set
            {
                Text3D_SetScale(Entity.ID, value);
            }
        }
        public float y_offset
        {
            get
            {
                return Text3D_GetYOffset(Entity.ID);
            }
            set
            {
                Text3D_SetYOffset(Entity.ID, value);
            }
        }
        Vector3 color
        {
            get
            {
                string result = Text3D_GetColor(Entity.ID);
                string[] split = result.Split(" ");
                return new Vector3(float.Parse(split[0]) * 255, float.Parse(split[1]) * 255, float.Parse(split[2]) * 255);
            }
            set
            {
                Text3D_SetColor(Entity.ID, value.x / 255, value.y / 255, value.z / 255);
            }
        }
    }
}