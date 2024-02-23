using Vault;

public class MyScript : Entity
{
    private void OnStart(string ID)
    {
        SetObjectID(ID);
    }

    private void OnUpdate()
    {
        transform.rotation.y += Time.deltaTime;
    }
}