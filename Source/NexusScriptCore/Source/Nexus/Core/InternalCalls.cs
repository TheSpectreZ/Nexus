using System;
using System.Runtime.CompilerServices;

namespace Nexus
{
    public static class InternalCalls
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_IsKeyPressed(UInt16 keyCode);
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_IsMouseButtonPressed(UInt16 keyCode);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Entity_HasComponent(ulong entityID,Type ComponentType);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetTranslation(ulong entityID, out Vector3 translation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetTranslation(ulong entityID, ref Vector3 translation);
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetRotation(ulong entityID, out Vector3 rotation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetRotation(ulong entityID, ref Vector3 rotation);
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetScale(ulong entityID, out Vector3 scale);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetScale(ulong entityID, ref Vector3 scale);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float RigidBodyComponent_GetMass(ulong entityID);
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidBodyComponent_SetMass(ulong entityID, ref float mass);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float RigidBodyComponent_GetFriction(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidBodyComponent_SetFriction(ulong entityID, ref float friction);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float RigidBodyComponent_GetRestitution(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidBodyComponent_SetRestitution(ulong entityID, ref float restitution);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool RigidBodyComponent_IsSimulating(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidBodyComponent_Simulate(ulong entityID, ref bool simulate);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void BoxColliderComponent_GetHalfExtent(ulong entityID, out Vector3 halfextent);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void BoxColliderComponent_SetHalfExtent(ulong entityID, ref Vector3 halfextent);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float SphereColliderComponent_GetRadius(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SphereColliderComponent_SetRadius(ulong entityID, ref float radius);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float CapsuleColliderComponent_GetTopRadius(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CapsuleColliderComponent_SetTopRadius(ulong entityID, ref float radius);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float CapsuleColliderComponent_GetBottomRadius(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CapsuleColliderComponent_SetBottomRadius(ulong entityID, ref float radius);
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float CapsuleColliderComponent_GetHalfHeight(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CapsuleColliderComponent_SetHalfHeight(ulong entityID, ref float radius);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float CylinderColliderComponent_GetRadius(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CylinderColliderComponent_SetRadius(ulong entityID, ref float radius);
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float CylinderColliderComponent_GetHalfHeight(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CylinderColliderComponent_SetHalfHeight(ulong entityID, ref float radius);

    }
}
