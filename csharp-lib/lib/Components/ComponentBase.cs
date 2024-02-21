using System;
using System.Runtime.CompilerServices;
using Vault;

namespace Vault
{
    public abstract class Component
    {
        public Entity Entity { get; internal set; }
    }
}