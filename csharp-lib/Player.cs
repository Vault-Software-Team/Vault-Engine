using Vault;

public class Player : Entity
{
    Rigidbody2D rigidbody;
    SpriteRenderer bg;

    string[] sprites = {
        "./assets/textures/meme1.jpg",
        "./assets/textures/meme2.png",
        "./assets/textures/meme3.png",
        "./assets/textures/meme4.png",
        "./assets/textures/meme5.png",
        "./assets/textures/meme5.png"
    };

    private void OnStart(string ID)
    {
        SetObjectID(ID);
        rigidbody = GetComponent<Rigidbody2D>();
        bg = GetByID(GameObject.GetIDByName("Background")).GetComponent<SpriteRenderer>();
    }

    private void OnUpdate()
    {
        if (Input.IsKeyDown(Input.KEY_SPACE))
        {
            Audio2D.PlaySound("./assets/music/splash.mp3");
            bg.texture = sprites[Mathf.RandomRange(0, sprites.Length - 1)];
            bg.color = new Vector3(1, 1, 1);
        }

        if (Input.IsKeyDown(Input.KEY_W) || Input.IsKeyDown(Input.KEY_SPACE))
        {
            rigidbody.SetVelocity(rigidbody.velocity.x, 3);
        }

        if (transform.position.y < -3 || transform.position.y > 3)
        {
            // Debug.Log("You fucking died loser");

            Scene.LoadScene("./assets/scenes/Game.vault");
        }

    }

    private void OnCollisionEnter2D(string ID)
    {
        // Debug.Log("You fucking died loser");

        Scene.LoadScene("./assets/scenes/Game.vault");
    }
}