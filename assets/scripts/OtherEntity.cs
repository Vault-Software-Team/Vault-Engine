using Vault;

public class EntityGet : Entity
{
    private void OnStart(string ID)
    {
        SetObjectID(ID);
    }

    private void OnUpdate()
    {
        if (Input.IsKeyDown(Input.KEY_ENTER))
        {
            Scene.LoadScene("./assets/scenes/Game.vault");
        }

        if (Input.IsKeyDown(Input.KEY_T))
        {
            GameObject.InstantiatePrefab("./assets/music_prefab.prefab");
        }
    }
}

