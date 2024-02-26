using System;

namespace Vault
{
    public class GameObject
    {
        public static string GetIDByName(string name)
        {
            return InternalCalls.GameObject_GetIDByName(name);
        }

        public static string GetIDByTag(string tag)
        {
            return InternalCalls.GameObject_GetIDByTag(tag);
        }
    }
}