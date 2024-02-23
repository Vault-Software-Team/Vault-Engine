using System;
using System.Runtime.CompilerServices;

namespace Vault
{
    public abstract class Component
    {
        public Entity Entity { get; internal set; }
    }
}