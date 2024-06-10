using Vault;

public class EntityGet : Entity
{
    OtherEntity otherEntity;

    void OnInit(string ID)
    {
        SetObjectID(ID);
    }

    private void OnStart(string ID)
    {
        otherEntity = GetByID(GameObject.GetIDByName("TF2 Logo")).As<OtherEntity>();
        otherEntity.PrintMyName();
    }

    private void OnUpdate()
    {

    }
}

