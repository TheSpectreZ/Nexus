using System;

namespace Nexus
{
    public class Entity
    {
        public virtual void OnCreate() { }
        public virtual void OnUpdate(float ts) { }
        public virtual void OnDestroy() { }
    }
}
