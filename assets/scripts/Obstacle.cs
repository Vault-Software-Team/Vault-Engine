using System.Runtime.CompilerServices;
using Vault;

public class Obstacle : Entity
{
    Rigidbody2D rigidbody;
    float maxX = -3.70f;
    float startX = 3.70f;

    // x = top, y = bottom


    private void OnStart(string ID)
    {
        SetObjectID(ID);
        rigidbody = GetComponent<Rigidbody2D>();
    }

    private void OnUpdate()
    {
        rigidbody.SetVelocity(-4, rigidbody.velocity.y);


        // if (transform.position.x <= maxX)
        // {
        //     transform.position.x = startX;
        // }
    }
}