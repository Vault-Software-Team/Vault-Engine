using Vault;

public class DebugCommand : DevConsole.Command
{
    public string name = "faggot";

    public void Init()
    {
        DevConsole.CommandRegistry.commands[name] = this;
    }


    public void Execute(string command, string[] arguments, string connected_arguments)
    {
        Debug.Log($"Command Executed: '{command} {connected_arguments}'");
    }
}