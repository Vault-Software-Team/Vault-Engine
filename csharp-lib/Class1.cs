using Vault;

public class MyScript : Entity
{
    Rigidbody2D rigidbody;
    private void OnStart(string ID)
    {
        SetObjectID(ID);
        rigidbody = GetComponent<Rigidbody2D>();
    }

    private void OnUpdate()
    {
        if (Input.IsKeyDown(Input.KEY_Q))
        {
            Audio2D.PlayMusic("../assets/yam.mp3", 1, true);
        }

        if (Input.IsKeyDown(Input.KEY_E))
        {
            Audio2D.StopMusic();
        }
    }
}