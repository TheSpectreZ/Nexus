using System;
using System.Runtime.CompilerServices;

namespace Nexus
{
    public abstract class Component
    {
        public Entity Entity { get; internal set; }
    }

    public class TransformComponent : Component
    {
        public Vector3 Position 
        {
            get
            {
                InternalCalls.TransformComponent_GetTranslation(Entity.ID, out Vector3 Position);
                return Position;
            }
            set
            {
                InternalCalls.TransformComponent_SetTranslation(Entity.ID, ref value);
            }
        }
        public Vector3 Rotation 
        {
            get
            {
                InternalCalls.TransformComponent_GetRotation(Entity.ID, out Vector3 Rotation);
                return Rotation;
            }
            set
            {
                InternalCalls.TransformComponent_SetRotation(Entity.ID, ref value);
            }
        }
        public Vector3 Scale 
        {
            get
            {
                InternalCalls.TransformComponent_GetScale(Entity.ID, out Vector3 Scale);
                return Scale;
            }
            set
            {
                InternalCalls.TransformComponent_SetScale(Entity.ID, ref value);
            }
        }
    }
}
