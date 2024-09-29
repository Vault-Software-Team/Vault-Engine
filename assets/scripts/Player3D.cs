using Vault;
public class Player3D : Entity
{
    Rigidbody2D rigidbody;

    void OnInit(string ID)
    {
        SetObjectID(ID);
    }

    private void OnStart(string ID)
    {
        rigidbody = GetComponent<Rigidbody2D>();
    }

    private void OnUpdate()
    {
        if (Input.IsKeyDown(Input.KEY_W) || Input.IsKeyDown(Input.KEY_SPACE))
        {
            rigidbody.SetVelocity(rigidbody.velocity.x, 3);
        }

        if (transform.position.y < -0.17 || transform.position.y > 4.3)
        {
            // Debug.Log("You fucking died loser");

            // Scene.LoadScene("./assets/scenes/3dflappy.vault");
        }
    }
}
