using System;
using System.Runtime.CompilerServices;
using Nexus;

namespace Nexus
{
    public class Entity
    {
        protected Entity() { ID = 0; }
        internal Entity(ulong id)
        {
            ID = id;
        }

        public readonly ulong ID;

        public virtual void OnCreate() { }
        public virtual void OnUpdate(float ts) { }
        public virtual void OnDestroy() { }
        public bool HasComponent<T>() where T : Component, new()
        {
            Type ComponentType = typeof(T);
            return InternalCalls.Entity_HasComponent(ID,ComponentType);
        }
        public T GetComponent<T>() where T : Component, new()
        {
            if (!HasComponent<T>())
                return null;

            T Component = new T() { Entity = this };
            return Component;
        }
    }
}
