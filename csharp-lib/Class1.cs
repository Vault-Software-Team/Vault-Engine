using Vault;

public class MyScript : Entity
{
    SpriteRenderer sprite;
    float speed;

    private void OnStart(string ID)
    {
        SetObjectID(ID);
        sprite = GetComponent<SpriteRenderer>();
    }

    private void OnUpdate()
    {
        if (Input.IsKeyDown(Input.KEY_Q))
        {
            sprite.texture = "../assets/k7.png";
        }
    }

    private void OnCollisionEnter2D(string ID)
    {
        Entity e = GetByID(ID);
        Debug.Log($"{name} collided with {e.name}");
    }

    private void OnCollisionExit2D(string ID)
    {
        Entity e = GetByID(ID);
        Debug.Log($"{name} exited with {e.name}");
    }
}