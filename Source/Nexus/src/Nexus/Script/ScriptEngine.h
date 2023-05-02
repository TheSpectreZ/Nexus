#pragma once
#include "Scene/Scene.h"

extern "C" {
	typedef struct _MonoDomain MonoDomain;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoImage MonoImage;
	typedef struct _MonoMethod MonoMethod;
}

namespace Nexus
{
	class ScriptInstance
	{
		friend class ScriptEngine;
	public:
		ScriptInstance() = default;
		ScriptInstance(const std::string& ClassName);

		void InVokeOnCreate();
		void InVokeOnUpdate(float ts);
		void InVokeOnDestroy();
	private:
		MonoClass* m_Class;
		MonoObject* m_Object;

		MonoMethod* m_OnCreate;
		MonoMethod* m_OnUpdate;
		MonoMethod* m_OnDestroy;
	};

	class ScriptEngine
	{
		friend class ScriptInstance;
		friend class ScriptGlue;
		static ScriptEngine* s_Instance;
	public:
		static void Init();
		static void Shut();

		static void OnSceneStart(Ref<Scene> scene);
		static void OnSceneUpdate(float ts);
		static void OnSceneStop();

		static Ref<Scene> GetSceneContext() { return s_Instance->m_scene; }
	private:
		void InitMono();
		void ShutdownMono();
		
		void InitAssembly();

		char* ReadBytes(const std::string& filepath, uint32_t* outSize);
		void PrintAssemblyTypes(MonoAssembly* assembly,const char* name);
		MonoAssembly* LoadAssembly(const std::string& assemblyPath);

		void LoadCoreAssemblyClasses();
	private:
		Ref<Scene> m_scene;

		MonoDomain* m_RootDomain;
		MonoDomain* m_AppDomain;
		MonoAssembly* m_CoreAssembly;
		MonoImage* m_CoreAssemblyImage;

		MonoClass* m_EntityBaseClass;
		MonoMethod* m_EntityBaseConstructor;

		std::unordered_map<std::string, MonoClass*> m_EntityClasses;
		std::unordered_map<uint64_t, ScriptInstance> m_EntityScriptInstances;
	};

}
