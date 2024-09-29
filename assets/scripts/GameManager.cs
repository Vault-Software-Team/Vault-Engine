using Vault;

public class GameManager : Entity
{
    public static GameManager instance = null;


    Vector2[] obstacle_positions = {
        new Vector2(4.47f, -1.06f),
        new Vector2(3.70f, -1.94f),
        new Vector2(5.85f, 0.06f),
        new Vector2(4.30f, -1.27f),
    };

    Entity oBottom, oTop, score;

    int c_score = 0;

    Text3D score_text;

    void OnInit(string ID)
    {
        SetObjectID(ID);
    }

    private void OnStart(string ID)
    {
        instance = this;

        oTop = GetByID(GameObject.GetIDByName("Obstacle Top"));
        oBottom = GetByID(GameObject.GetIDByName("Obstacle Bottom"));
        score = GetByID(GameObject.GetIDByName("Score Text"));
        score_text = score.GetComponent<Text3D>();

        // Audio2D.PlayMusic("./assets/music/fchiptune.wav", 0.5f, true);

    }

    private void OnUpdate()
    {
        if (oTop.transform.position.x <= -6.00)
        {
            c_score++;
            int index = Mathf.RandomRange(0, obstacle_positions.Length - 1);

            oTop.GetComponent<Rigidbody2D>().SetPosition(6.00f, obstacle_positions[index].x);
            oBottom.GetComponent<Rigidbody2D>().SetPosition(6.00f, obstacle_positions[index].y);
        }

        score_text.text = "SCORE: " + Format.ToString(c_score);
    }
}













