using Vault;

public class CameraTest : Entity
{
    Camera cam;

    void OnInit(string ID)
    {
        SetObjectID(ID);
    }

    private void OnStart(string ID)
    {
        cam = GetComponent<Camera>();
    }

    private void OnUpdate()
    {
        cam.FOV = 120;
    }
}

