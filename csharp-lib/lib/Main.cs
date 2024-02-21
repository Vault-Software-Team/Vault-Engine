using System;

namespace Vault {
    public class Entity {
        public string ID;
        protected virtual void OnStart(string ID) {}
        protected virtual void OnUpdate() {}

        public T GetComponent<T>() where T : Component, new()
        {
            T component = new T() { Entity = this };
            return (T)(object)component;

            return null;
        }
    }
}