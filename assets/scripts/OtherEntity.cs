using Vault;

public class OtherEntity : Entity
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

    public void PrintMyName()
    {
        Debug.Log("My name is " + name);
    }
}

