using Vault;

public class GameManager : Entity
{
    public static GameManager instance = null;


    Vector2[] obstacle_positions = {
        new Vector2(2.87f, -1.60f),
        new Vector2(1.65f, -2.90f),
        new Vector2(2.26f, -2.10f),
        new Vector2(3.43f, -1.16f),
    };

    Entity oBottom, oTop, score;
    Text3D score_text;

    int c_score = 0;

    private void OnStart(string ID)
    {
        SetObjectID(ID);
        instance = this;

        oTop = GetByID(GameObject.GetIDByName("Obstacle"));
        oBottom = GetByID(GameObject.GetIDByName("Obstacle Bottom"));
        score = GetByID(GameObject.GetIDByName("Score Text"));
        score_text = score.GetComponent<Text3D>();

        Audio2D.PlayMusic("./assets/music/fchiptune.wav", 0.5f, true);

    }

    private void OnUpdate()
    {
        if (oTop.transform.position.x <= -4.02f)
        {
            c_score++;
            int index = Mathf.RandomRange(0, obstacle_positions.Length - 1);

            oTop.GetComponent<Rigidbody2D>().SetPosition(4.02f, obstacle_positions[index].x);
            oBottom.GetComponent<Rigidbody2D>().SetPosition(4.02f, obstacle_positions[index].y);
        }

        score_text.text = "Score: " + Format.ToString(c_score);
    }
}













