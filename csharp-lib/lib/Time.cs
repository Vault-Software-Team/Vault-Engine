using System;
using System.Runtime.CompilerServices;

namespace Vault
{
    public class Time
    {
        public static float deltaTime
        {
            get { return InternalCalls.Time_GetDeltaTime(); }
            set { }
        }
    }
}