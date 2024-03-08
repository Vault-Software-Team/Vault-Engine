using System;
using System.Runtime.CompilerServices;

namespace Vault
{
    public class Debug
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Log(string content);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Error(string content);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Warning(string content);
    }
}