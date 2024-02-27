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
        Debug.Log("Velocity: " + Format.ToString(rigidbody.velocity));
    }
}