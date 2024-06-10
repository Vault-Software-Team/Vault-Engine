using Vault;

public class EntityGet : Entity
{
    OtherEntity otherEnt = null;

    void OnInit(string ID)
    {
        SetObjectID(ID);
    }

    private void OnStart(string ID)
    {
        otherEnt = GetByID(GameObject.GetIDByName("Text")).As<OtherEntity>();
        if (otherEnt == null) Debug.Log("its null bruh");
        else
            otherEnt.PrintMyName();
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

