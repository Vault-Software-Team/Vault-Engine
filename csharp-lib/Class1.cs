using Vault;

public class MyScript : Entity
{
    Text3D text;

    private void OnStart(string ID)
    {
        SetObjectID(ID);
        text = GetComponent<Text3D>();
    }

    private void OnUpdate()
    {
        if (Input.IsKeyDown(Input.KEY_Q))
        {
            text.text = "Q Pressed!";
        }

        if (Input.IsKeyDown(Input.KEY_W))
        {
            text.text = "W Pressed!";
        }
    }
}