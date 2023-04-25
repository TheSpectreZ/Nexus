#pragma once

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
	class ScriptEngine
	{
		friend class ScriptInstance;
		static ScriptEngine* s_Instance;
	public:
		static void Init();
		static void Shut();
	private:
		void InitMono();
		void ShutdownMono();
		
		char* ReadBytes(const std::string& filepath, uint32_t* outSize);
		void PrintAssemblyTypes(MonoAssembly* assembly,const char* name);
		MonoAssembly* LoadAssembly(const std::string& assemblyPath);

		void LoadCoreAssemblyClasses();
	private:
		MonoDomain* m_RootDomain;
		MonoDomain* m_AppDomain;
		MonoAssembly* m_CoreAssembly;
		MonoImage* m_CoreAssemblyImage;

		std::unordered_map<std::string, MonoClass*> m_EntityClasses;
	};

	class ScriptInstance
	{
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
}
