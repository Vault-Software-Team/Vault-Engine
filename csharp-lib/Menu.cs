using Vault;

public class MainMenu : Entity
{
    private void OnStart(string ID)
    {
        SetObjectID(ID);
    }

    private void OnUpdate()
    {
        if (Input.IsKeyDown(Input.KEY_ENTER))
        {
            Scene.LoadScene("../assets/scene.vault");
        }
    }
}