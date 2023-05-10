#include "nxpch.h"

#include "ScriptGlue.h"
#include "ScriptEngine.h"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"

#include "Scene/Entity.h"
#include "Scene/Component.h"

#include "Core/Input.h"

#define REGISTER_INTERNAL_CALL(method) mono_add_internal_call("Nexus.InternalCalls::"#method,(void*)(InternalCalls::method))

namespace Nexus
{
	std::unordered_map<MonoType*, std::function<bool(Entity)>> s_HasComponentFuncs;

	template<typename Component>
	static void RegisterComponent(MonoImage* Image)
	{
		std::string_view name = typeid(Component).name();
		size_t it = name.find_last_of(":");
		std::string_view component = name.substr(it+1);
		std::string ComponentName = fmt::format("Nexus.{}Component", component);

		MonoType* managedType = mono_reflection_type_from_name(ComponentName.data(), Image);
		NEXUS_ASSERT((managedType == nullptr), "ScriptEngine: Could Find Component");

		s_HasComponentFuncs[managedType] = [](Entity e) {return e.HasComponent<Component>(); };
	}
}

namespace Nexus::InternalCalls
{

#pragma region Input
	
	bool Input_IsKeyPressed(uint16_t key)
	{
		return Input::IsKeyPressed(key);
	}

	bool Input_IsMouseButtonPressed(uint16_t button)
	{
		return Input::IsMouseButtonPressed(button);
	}

#pragma endregion

#pragma region Entity
	
	bool Entity_HasComponent(UUID id, MonoReflectionType* type)
	{
		Entity en = ScriptEngine::GetSceneContext()->GetEntityWithUUID(id);
		NEXUS_ASSERT((!en), "Entity is Null");

		MonoType* ty = mono_reflection_type_get_type(type);
		NEXUS_ASSERT((ty == nullptr), "MonoType is nullptr");

		NEXUS_ASSERT((s_HasComponentFuncs.find(ty) == s_HasComponentFuncs.end()), "Component Type Not Registered");
		return s_HasComponentFuncs[ty](en);
	}

#pragma endregion

#pragma region TransformComponent

	void TransformComponent_GetTranslation(UUID id, glm::vec3& translation)
	{
		Entity en = ScriptEngine::GetSceneContext()->GetEntityWithUUID(id);
		translation = en.GetComponent<Component::Transform>().Translation;
	}

	void TransformComponent_SetTranslation(UUID id, glm::vec3& translation)
	{
		Entity en = ScriptEngine::GetSceneContext()->GetEntityWithUUID(id);
		auto& Transform = en.GetComponent<Component::Transform>();
		Transform.Translation = translation;
	}

	void TransformComponent_GetRotation(UUID id, glm::vec3& rotation)
	{
		Entity en = ScriptEngine::GetSceneContext()->GetEntityWithUUID(id);
		rotation = en.GetComponent<Component::Transform>().GetRotationEuler();
	}

	void TransformComponent_SetRotation(UUID id, glm::vec3& rotation)
	{
		Entity en = ScriptEngine::GetSceneContext()->GetEntityWithUUID(id);
		auto& Transform = en.GetComponent<Component::Transform>();
		Transform.SetRotationEuler(rotation);
	}

	void TransformComponent_GetScale(UUID id, glm::vec3& scale)
	{
		Entity en = ScriptEngine::GetSceneContext()->GetEntityWithUUID(id);
		scale = en.GetComponent<Component::Transform>().Scale;
	}

	void TransformComponent_SetScale(UUID id, glm::vec3& scale)
	{
		Entity en = ScriptEngine::GetSceneContext()->GetEntityWithUUID(id);
		auto& Transform = en.GetComponent<Component::Transform>();
		Transform.Scale = scale;
	}
#pragma endregion

#pragma region RigidBodyComponent

	float RigidBodyComponent_GetMass(UUID id)
	{
		Entity en = ScriptEngine::GetSceneContext()->GetEntityWithUUID(id);
		return en.GetComponent<Component::RigidBody>().mass;
	}

	void RigidBodyComponent_SetMass(UUID id, float& mass)
	{
		Entity en = ScriptEngine::GetSceneContext()->GetEntityWithUUID(id);
		auto& rb = en.GetComponent<Component::RigidBody>();
		rb.mass = mass;
	}

	float RigidBodyComponent_GetFriction(UUID id)
	{
		Entity en = ScriptEngine::GetSceneContext()->GetEntityWithUUID(id);
		return en.GetComponent<Component::RigidBody>().friction;
	}

	void RigidBodyComponent_SetFriction(UUID id, float& friction)
	{
		Entity en = ScriptEngine::GetSceneContext()->GetEntityWithUUID(id);
		auto& rb = en.GetComponent<Component::RigidBody>();
		rb.friction = friction;
	}

	float RigidBodyComponent_GetRestitution(UUID id)
	{
		Entity en = ScriptEngine::GetSceneContext()->GetEntityWithUUID(id);
		return en.GetComponent<Component::RigidBody>().restitution;
	}

	void RigidBodyComponent_SetRestitution(UUID id, float& res)
	{
		Entity en = ScriptEngine::GetSceneContext()->GetEntityWithUUID(id);
		auto& bc = en.GetComponent<Component::RigidBody>();
		bc.restitution = res;
	}
	
	bool RigidBodyComponent_IsSimulating(UUID id)
	{
		Entity en = ScriptEngine::GetSceneContext()->GetEntityWithUUID(id);
		return en.GetComponent<Component::RigidBody>().Simulate;
	}

	void RigidBodyComponent_Simulate(UUID id, bool& sim) {
		Entity en = ScriptEngine::GetSceneContext()->GetEntityWithUUID(id);
		auto& rb = en.GetComponent<Component::RigidBody>();
		rb.Simulate = sim;
	}

#pragma endregion

#pragma region BoxColliderComponent

	void BoxColliderComponent_GetHalfExtent(UUID id, glm::vec3& halfextent)
	{
		Entity en = ScriptEngine::GetSceneContext()->GetEntityWithUUID(id);
		halfextent = en.GetComponent<Component::BoxCollider>().HalfExtent;
	}

	void BoxColliderComponent_SetHalfExtent(UUID id, glm::vec3& halfextent)
	{
		Entity en = ScriptEngine::GetSceneContext()->GetEntityWithUUID(id);
		auto& bc = en.GetComponent<Component::BoxCollider>();
		bc.HalfExtent = halfextent;
	}

#pragma endregion

#pragma region SphereColliderComponent

	float SphereColliderComponent_GetRadius(UUID id)
	{
		Entity en = ScriptEngine::GetSceneContext()->GetEntityWithUUID(id);
		return en.GetComponent<Component::SphereCollider>().Radius;
	}
	
	void SphereColliderComponent_SetRadius(UUID id,float& radius)
	{
		Entity en = ScriptEngine::GetSceneContext()->GetEntityWithUUID(id);
		en.GetComponent<Component::SphereCollider>().Radius = radius;
	}

#pragma endregion

#pragma region CapsuleColliderComponent
	
	float CapsuleColliderComponent_GetTopRadius(UUID id)
	{
		Entity en = ScriptEngine::GetSceneContext()->GetEntityWithUUID(id);
		return en.GetComponent<Component::CapsuleCollider>().TopRadius;
	}
	
	void CapsuleColliderComponent_SetTopRadius(UUID id,float& radius)
	{
		Entity en = ScriptEngine::GetSceneContext()->GetEntityWithUUID(id);
		en.GetComponent<Component::CapsuleCollider>().TopRadius = radius;
	}

	float CapsuleColliderComponent_GetBottomRadius(UUID id)
	{
		Entity en = ScriptEngine::GetSceneContext()->GetEntityWithUUID(id);
		return en.GetComponent<Component::CapsuleCollider>().BottomRadius;
	}

	void CapsuleColliderComponent_SetBottomRadius(UUID id, float& radius)
	{
		Entity en = ScriptEngine::GetSceneContext()->GetEntityWithUUID(id);
		en.GetComponent<Component::CapsuleCollider>().BottomRadius = radius;
	}
	
	float CapsuleColliderComponent_GetHalfHeight(UUID id)
	{
		Entity en = ScriptEngine::GetSceneContext()->GetEntityWithUUID(id);
		return en.GetComponent<Component::CapsuleCollider>().HalfHeight;
	}

	void CapsuleColliderComponent_SetHalfHeight(UUID id, float& HalfHeight)
	{
		Entity en = ScriptEngine::GetSceneContext()->GetEntityWithUUID(id);
		en.GetComponent<Component::CapsuleCollider>().HalfHeight = HalfHeight;
	}

#pragma endregion

#pragma region CylinderColliderComponent

	float CylinderColliderComponent_GetRadius(UUID id)
	{
		Entity en = ScriptEngine::GetSceneContext()->GetEntityWithUUID(id);
		return en.GetComponent<Component::CylinderCollider>().Radius;
	}
	
	void CylinderColliderComponent_SetRadius(UUID id,float& radius)
	{
		Entity en = ScriptEngine::GetSceneContext()->GetEntityWithUUID(id);
		en.GetComponent<Component::CylinderCollider>().Radius = radius;
	}
	
	float CylinderColliderComponent_GetHalfHeight(UUID id)
	{
		Entity en = ScriptEngine::GetSceneContext()->GetEntityWithUUID(id);
		return en.GetComponent<Component::CylinderCollider>().HalfHeight;
	}
	
	void CylinderColliderComponent_SetHalfHeight(UUID id, float& halfheight)
	{
		Entity en = ScriptEngine::GetSceneContext()->GetEntityWithUUID(id);
		en.GetComponent<Component::CylinderCollider>().HalfHeight = halfheight;
	}

#pragma endregion

}

void Nexus::ScriptGlue::BindComponents()
{
	MonoImage* image = ScriptEngine::s_Instance->m_CoreAssemblyImage;

	RegisterComponent<Component::Transform>(image);
	RegisterComponent<Component::RigidBody>(image);
	RegisterComponent<Component::BoxCollider>(image);
}

void Nexus::ScriptGlue::BindInternalCalls()
{
	REGISTER_INTERNAL_CALL(Input_IsKeyPressed);
	REGISTER_INTERNAL_CALL(Input_IsMouseButtonPressed);

	REGISTER_INTERNAL_CALL(Entity_HasComponent);

	REGISTER_INTERNAL_CALL(TransformComponent_GetTranslation);
	REGISTER_INTERNAL_CALL(TransformComponent_SetTranslation);
	REGISTER_INTERNAL_CALL(TransformComponent_GetRotation);
	REGISTER_INTERNAL_CALL(TransformComponent_SetRotation);
	REGISTER_INTERNAL_CALL(TransformComponent_GetScale);
	REGISTER_INTERNAL_CALL(TransformComponent_SetScale);

	REGISTER_INTERNAL_CALL(RigidBodyComponent_GetRestitution);
	REGISTER_INTERNAL_CALL(RigidBodyComponent_SetRestitution);
	REGISTER_INTERNAL_CALL(RigidBodyComponent_GetFriction);
	REGISTER_INTERNAL_CALL(RigidBodyComponent_SetFriction);
	REGISTER_INTERNAL_CALL(RigidBodyComponent_GetMass);
	REGISTER_INTERNAL_CALL(RigidBodyComponent_SetMass);
	REGISTER_INTERNAL_CALL(RigidBodyComponent_IsSimulating);
	REGISTER_INTERNAL_CALL(RigidBodyComponent_Simulate);

	REGISTER_INTERNAL_CALL(BoxColliderComponent_GetHalfExtent);
	REGISTER_INTERNAL_CALL(BoxColliderComponent_SetHalfExtent);
	
	REGISTER_INTERNAL_CALL(SphereColliderComponent_GetRadius);
	REGISTER_INTERNAL_CALL(SphereColliderComponent_SetRadius);

	REGISTER_INTERNAL_CALL(CapsuleColliderComponent_GetTopRadius);
	REGISTER_INTERNAL_CALL(CapsuleColliderComponent_SetTopRadius);
	REGISTER_INTERNAL_CALL(CapsuleColliderComponent_GetBottomRadius);
	REGISTER_INTERNAL_CALL(CapsuleColliderComponent_SetBottomRadius);
	REGISTER_INTERNAL_CALL(CapsuleColliderComponent_GetHalfHeight);
	REGISTER_INTERNAL_CALL(CapsuleColliderComponent_SetHalfHeight);

	REGISTER_INTERNAL_CALL(CylinderColliderComponent_GetRadius);
	REGISTER_INTERNAL_CALL(CylinderColliderComponent_SetRadius);
	REGISTER_INTERNAL_CALL(CylinderColliderComponent_GetHalfHeight);
	REGISTER_INTERNAL_CALL(CylinderColliderComponent_SetHalfHeight);

}