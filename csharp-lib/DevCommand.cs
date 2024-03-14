using System;
using System.Collections.Generic;

namespace DevConsole
{
    public class CommandRegistry
    {
        public static Dictionary<string, Command> commands;

        public static void Execute(string command, string args)
        {
            if (commands[command] != null)
            {
                Vault.Debug.Error("Invalid Command \"" + command + "\"");
                return;
            }

            commands[command].Execute(command, args.Split(" "), args);
        }
    }

    public class Command
    {
        public string name = "";

        public virtual void Execute(string command, string[] arguments, string connected_arguments)
        {
            Vault.Debug.Log($"Command Executed: '{command} {connected_arguments}'");
        }
    }
}