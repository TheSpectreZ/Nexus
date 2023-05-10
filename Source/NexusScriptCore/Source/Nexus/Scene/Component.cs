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

    public class RigidBodyComponent : Component
    {
        public float Mass
        {
            get => InternalCalls.RigidBodyComponent_GetMass(Entity.ID);
            set => InternalCalls.RigidBodyComponent_SetMass(Entity.ID, ref value);
        }
        public float Friction
        {
            get => InternalCalls.RigidBodyComponent_GetFriction(Entity.ID);
            set => InternalCalls.RigidBodyComponent_SetFriction(Entity.ID, ref value);
        }
        public float Restitution
        {
            get => InternalCalls.RigidBodyComponent_GetRestitution(Entity.ID); 
            set => InternalCalls.RigidBodyComponent_SetRestitution(Entity.ID, ref value);
        }
        public bool Simulate
        {
            get => InternalCalls.RigidBodyComponent_IsSimulating(Entity.ID);
            set => InternalCalls.RigidBodyComponent_Simulate(Entity.ID, ref value);       
        }
    }

    public class BoxColliderComponent : Component 
    {
        public Vector3 HalfExtent
        {
            get
            {
                InternalCalls.BoxColliderComponent_GetHalfExtent(Entity.ID, out Vector3 extent);
                return extent;
            }
            set
            {
                InternalCalls.BoxColliderComponent_SetHalfExtent(Entity.ID, ref value);
            }
        }
    }

    public class SphereColliderComponent : Component
    {
        public float Radius
        {
            get => InternalCalls.SphereColliderComponent_GetRadius(Entity.ID);
            set => InternalCalls.SphereColliderComponent_SetRadius(Entity.ID, ref value);
        }
    }

    public class CapsuleColliderComponent : Component
    {
        public float HalfHeight
        {
            get => InternalCalls.CapsuleColliderComponent_GetHalfHeight(Entity.ID);
            set => InternalCalls.CapsuleColliderComponent_SetHalfHeight(Entity.ID, ref value);
        }
        public float TopRadius
        {
            get => InternalCalls.CapsuleColliderComponent_GetTopRadius(Entity.ID);
            set => InternalCalls.CapsuleColliderComponent_SetTopRadius(Entity.ID, ref value);
        }
        public float BottomRadius
        {
            get => InternalCalls.CapsuleColliderComponent_GetBottomRadius(Entity.ID);
            set => InternalCalls.CapsuleColliderComponent_SetBottomRadius(Entity.ID, ref value);
        }
    }
    public class CylinderColliderComponent : Component
    {
        public float Radius
        {
            get => InternalCalls.CylinderColliderComponent_GetRadius(Entity.ID);
            set => InternalCalls.CylinderColliderComponent_SetRadius(Entity.ID, ref value);
        }
        public float HalfHeight
        {
            get => InternalCalls.CylinderColliderComponent_GetHalfHeight(Entity.ID);
            set => InternalCalls.CylinderColliderComponent_SetHalfHeight(Entity.ID, ref value);
        }
    }
}
