using Vault;

public class OtherEntity : Entity
{
    void OnInit(string ID)
    {
        SetObjectID(ID);
    }

    void OnStart(string ID)
    {
    }

    void OnUpdate()
    {
    }

    public void PrintMyName()
    {
        Debug.Log("My name is " + name);
    }
}

