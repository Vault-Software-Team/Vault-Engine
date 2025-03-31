using System.Runtime.CompilerServices;
using Vault;

public class Obstacle : Entity
{

    Rigidbody2D rigidbody;
    float maxX = -6.00f;
    float startX = 6.00f;

    // x = top, y = bottom


    void OnInit(string ID)
    {
        SetObjectID(ID);
    }

    private void OnStart(string ID)
    {
        rigidbody = GetComponent<Rigidbody2D>();
    }

    private void OnUpdate()
    {
        rigidbody.SetVelocity(-6, rigidbody.velocity.y);


        // if (transform.position.x <= maxX)
        // {
        //     transform.position.x = startX;
        // }
    }
    
}
