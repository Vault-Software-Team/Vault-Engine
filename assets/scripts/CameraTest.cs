using Vault;

public class CameraTest : Entity
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

