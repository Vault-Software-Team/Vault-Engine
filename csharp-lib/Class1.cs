using Vault;

public class MyScript : Entity
{
    private void OnStart(string ID)
    {
        SetObjectID(ID);
    }

    private void OnUpdate()
    {
        if (Input.IsKeyDown(Input.KEY_Q))
        {
            Debug.Error("Q Pressed!");
        }
    }
}