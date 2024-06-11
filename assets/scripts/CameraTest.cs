using Vault;

public class CameraTest : Entity
{
    PointLight light;

    void OnInit(string ID)
    {
        SetObjectID(ID);
    }

    private void OnStart(string ID)
    {
        light = GetComponent<PointLight>();
    }

    private void OnUpdate()
    {
        light.color = new Vector3(1, 0, 0);
    }
}

