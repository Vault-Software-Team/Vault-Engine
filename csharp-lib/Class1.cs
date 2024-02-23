using System;
using Vault;

public class MyScript : Entity {
    void OnStart(string GameObjectID) {
        this.ID = GameObjectID;
        Console.WriteLine("ID: " + this.ID);
    }

    void OnUpdate() {
    }
}