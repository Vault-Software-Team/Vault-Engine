using Vault;

public class CameraTest : Entity
{

    void OnInit(string ID)
    {
        SetObjectID(ID);
    }

    Rigidbody3D rigidbody;

    private void OnStart(string ID)
    {
        rigidbody = GetComponent<Rigidbody3D>();
    }

    private void OnUpdate()
    {
        rigidbody.velocity = new Vector3(1, 0, 0);
    }

    void OnMouseEnter()
    {
        Debug.Log("Mouse Entered!");
    }

    void OnMouseExit()
    {
        Debug.Log("Mouse Left!");
    }
}

