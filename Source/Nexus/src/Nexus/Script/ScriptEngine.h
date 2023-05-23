#pragma once
#include "Scene/Scene.h"

extern "C" {
	typedef struct _MonoDomain MonoDomain;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoImage MonoImage;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoClassField MonoClassField;
}

namespace Nexus
{
	enum class ScriptFieldType
	{
		None, Float, Double, UInt, Int, Bool, Vec2, Vec3, Vec4
	};

	struct ScriptField
	{
		ScriptFieldType Type;
		MonoClassField* Field;
	};

	class ScriptClass
	{
		friend class ScriptInstance;
	public:
		ScriptClass() = default;
		ScriptClass(MonoClass* Class);

		std::unordered_map<std::string, ScriptField> GetFields() const { return m_Fields; }
	private:
		MonoClass* m_Class;
		MonoMethod* m_OnCreate;
		MonoMethod* m_OnUpdate;
		MonoMethod* m_OnDestroy;

		std::unordered_map<std::string, ScriptField> m_Fields;
	};

	class ScriptInstance
	{
		friend class ScriptEngine;
	public:
		ScriptInstance() = default;
		ScriptInstance(ScriptClass* Class);

		void InVokeOnCreate();
		void InVokeOnUpdate(float ts);
		void InVokeOnDestroy();

		ScriptClass* GetScriptClass() { return m_ScriptClass; }

		template<typename T>
		T GetFieldValue(const std::string& name)
		{
			if (!GetFieldValueInternal(name, s_FieldValueBuffer))
				return T();

			return *(T*)s_FieldValueBuffer;
		}

		void SetFieldValue(const std::string& name,void* value);
	private:
		bool GetFieldValueInternal(const std::string& name,void* buffer);

		MonoObject* m_Object;
		ScriptClass* m_ScriptClass;

		inline static char s_FieldValueBuffer[8];
	};

	class ScriptEngine
	{
		friend class ScriptClass;
		friend class ScriptInstance;
		friend class ScriptGlue;
		static ScriptEngine* s_Instance;
	public:
		static void Init();
		static void Shut();

		static void ReloadAssembly(const std::string& filepath);

		static void OnSceneStart(Ref<Scene> scene);
		static void OnSceneUpdate(float ts);
		static void OnSceneStop();

		static Ref<Scene> GetSceneContext() { return s_Instance->m_scene; }

		static ScriptInstance* GetEntityScriptInstance(uint64_t id) { return &s_Instance->m_EntityScriptInstances[id]; }

		static bool EntityClassExists(const std::string& name) { return s_Instance->m_EntityClasses.contains(name); }
		static bool EntityInstanceExists(uint64_t id) { return s_Instance->m_EntityScriptInstances.contains(id); }
	private:
		void InitMono();
		void ShutdownMono();
		
		void InitAssembly(const std::string& appAssemblyPath);

		char* ReadBytes(const std::string& filepath, uint32_t* outSize);
		void PrintAssemblyTypes(MonoAssembly* assembly,const char* name);
		MonoAssembly* LoadAssembly(const std::string& assemblyPath);

		void LoadAppAssemblyClasses();
	private:
		Ref<Scene> m_scene;

		MonoDomain* m_RootDomain;
		MonoDomain* m_AppDomain;

		MonoAssembly* m_CoreAssembly;
		MonoImage* m_CoreAssemblyImage;

		MonoAssembly* m_AppAssembly;
		MonoImage* m_AppAssemblyImage;

		MonoClass* m_EntityBaseClass;
		MonoMethod* m_EntityBaseConstructor;

		std::unordered_map<std::string, ScriptClass> m_EntityClasses;
		std::unordered_map<uint64_t, ScriptInstance> m_EntityScriptInstances;
	};

}
