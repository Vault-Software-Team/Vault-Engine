using System;
using System.Runtime.CompilerServices;
using Vault;

namespace Vault
{
    public class ModelAnimator : Component
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static string ModelAnimator_GetCurrentAnimation(string ID);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void ModelAnimator_SetCurrentAnimation(string ID, string value);

        public string currentAnimation
        {
            get
            {
                return ModelAnimator_GetCurrentAnimation(Entity.ID);
            }
            set
            {
                ModelAnimator_SetCurrentAnimation(Entity.ID, value);
            }
        }
    }
}