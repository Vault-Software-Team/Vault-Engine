using Vault;

public class MyScript : Entity
{
    private void OnStart(string ID)
    {
        SetObjectID(ID);
    }

    private void OnUpdate()
    {
        if (Input.IsKeyPressed(Input.KEY_Q))
        {
            Debug.Log("Adding prefab");
            Entity e = GameObject.InstantiatePrefab("../assets/sprite.prefab");
            Debug.Log("Prefab added: " + e.name + " " + e.ID);
        }
    }
}