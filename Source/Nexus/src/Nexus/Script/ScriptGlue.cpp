#include "nxpch.h"

#include "ScriptGlue.h"
#include "ScriptEngine.h"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"

#include "Scene/Entity.h"
#include "Scene/Component.h"

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

}

void Nexus::ScriptGlue::BindComponents()
{
	MonoImage* image = ScriptEngine::s_Instance->m_CoreAssemblyImage;

	RegisterComponent<Component::Transform>(image);
}

void Nexus::ScriptGlue::BindInternalCalls()
{
	REGISTER_INTERNAL_CALL(Entity_HasComponent);

	REGISTER_INTERNAL_CALL(TransformComponent_GetTranslation);
	REGISTER_INTERNAL_CALL(TransformComponent_SetTranslation);
	REGISTER_INTERNAL_CALL(TransformComponent_GetRotation);
	REGISTER_INTERNAL_CALL(TransformComponent_SetRotation);
	REGISTER_INTERNAL_CALL(TransformComponent_GetScale);
	REGISTER_INTERNAL_CALL(TransformComponent_SetScale);
}
