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

        public static Entity InstantiatePrefab(string path, string name = "__VAULT_DEFAULT_NAME__", string tag = "__VAULT_DEFAULT_TAG__", string id = "DEFAULT")
        {
            string _id = InternalCalls.GameObject_InstantiatePrefab(path, name, tag, id);
            return Entity.GetByID(_id);
        }
        public static Entity InstantiatePrefabWithProps(string path, Vector3 position = default, Vector3 rotation = default, Vector3 scale = default, string name = "__VAULT_DEFAULT_NAME__", string tag = "__VAULT_DEFAULT_TAG__", string id = "DEFAULT")
        {
            string _id = InternalCalls.GameObject_InstantiatePrefabWithProps(path, position.x, position.y, position.z, rotation.x, rotation.y, rotation.z, scale.x, scale.y, scale.z, name, tag, id);
            return Entity.GetByID(_id);
        }
    }
}