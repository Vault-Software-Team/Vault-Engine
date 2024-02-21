namespace Vault {
    public class Entity {
        string ID;
        private void SetObjectID(string ID) {
            this.ID = ID;
        }
        protected virtual void OnStart(string ID) {}
        protected virtual void OnUpdate() {}
    }
}