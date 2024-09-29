using Vault;
public class SpinOther : Entity
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
        transform.rotation.y -= 5 * Time.deltaTime;
    }
}
