using Vault;
public class Spin : Entity
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
        transform.rotation.y += 5 * Time.deltaTime;
    }
}
