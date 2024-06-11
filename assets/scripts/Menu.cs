using Vault;

public class MainMenu : Entity
{
    void OnInit(string ID)
    {
        SetObjectID(ID);
    }

    private void OnStart(string ID)
    {
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

