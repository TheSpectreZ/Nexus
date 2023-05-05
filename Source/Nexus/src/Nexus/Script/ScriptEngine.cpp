#include "nxpch.h"
#include "ScriptEngine.h"
#include "ScriptGlue.h"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"

#include "Scene/Entity.h"

Nexus::ScriptEngine* Nexus::ScriptEngine::s_Instance = nullptr;

void Nexus::ScriptEngine::Init()
{
    s_Instance = new ScriptEngine();
	s_Instance->InitMono();
    s_Instance->InitAssembly();

    ScriptGlue::BindComponents();
    ScriptGlue::BindInternalCalls();
}

void Nexus::ScriptEngine::Shut()
{
    s_Instance->ShutdownMono();
    delete s_Instance;
}

void Nexus::ScriptEngine::OnSceneStart(Ref<Scene> scene)
{
    s_Instance->m_scene = scene;;

    Nexus::Entity entity;
    auto view = scene->m_registry.view<Nexus::Component::Script>();
    for (auto& e : view)
    {
        entity = Nexus::Entity(e, scene.get());
        auto& id = entity.GetComponent<Nexus::Component::Identity>();
        auto& Script = entity.GetComponent<Nexus::Component::Script>();

        if (!s_Instance->m_EntityClasses.contains(Script.name))
            continue;

        Nexus::ScriptInstance inst(Script.name);

        void* param = &id.uuid;
        mono_runtime_invoke(s_Instance->m_EntityBaseConstructor, inst.m_Object, &param, nullptr);

        s_Instance->m_EntityScriptInstances[id.uuid] = inst;
        s_Instance->m_EntityScriptInstances[id.uuid].InVokeOnCreate();
    }
}

void Nexus::ScriptEngine::OnSceneUpdate(float ts)
{
    for (auto& [k, v] : s_Instance->m_EntityScriptInstances)
    {
        v.InVokeOnUpdate(ts);
    }
}

void Nexus::ScriptEngine::OnSceneStop()
{
    for (auto& [k, v] : s_Instance->m_EntityScriptInstances)
    {
        v.InVokeOnDestroy();
    }
    s_Instance->m_EntityScriptInstances.clear();
    s_Instance->m_scene = nullptr;
}

void Nexus::ScriptEngine::InitMono()
{
    mono_set_assemblies_path("mono/lib");

    MonoDomain* rootDomain = mono_jit_init("NexusJITRuntime");
    NEXUS_ASSERT((rootDomain == nullptr), "Failed To Initialize Mono JIT");
    m_RootDomain = rootDomain;

    char friendlyName[] = "NexusAppDomain";
    m_AppDomain = mono_domain_create_appdomain(friendlyName, nullptr);
    mono_domain_set(m_AppDomain, true);
}

void Nexus::ScriptEngine::ShutdownMono()
{
    m_AppDomain = nullptr;

    mono_jit_cleanup(m_RootDomain);
    m_RootDomain = nullptr;
}

void Nexus::ScriptEngine::InitAssembly()
{
    m_CoreAssembly = LoadAssembly("Resources/Scripts/NexusScriptCore.dll");
    m_CoreAssemblyImage = mono_assembly_get_image(m_CoreAssembly);

    PrintAssemblyTypes(m_CoreAssembly, "Core Assembly");
    
    // Loading Classes
    LoadCoreAssemblyClasses();
}

void Nexus::ScriptEngine::PrintAssemblyTypes(MonoAssembly* assembly,const char* name)
{
    MonoImage* image = mono_assembly_get_image(assembly);
    const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
    int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

    NEXUS_LOG_INFO("Printing Assembly Types: {0}", name);
    for (int32_t i = 0; i < numTypes; i++)
    {
        uint32_t cols[MONO_TYPEDEF_SIZE];
        mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

        const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
        const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

        NEXUS_LOG_TRACE("{0}.{1}", nameSpace, name);
    }
    NEXUS_LOG_INFO("");
}

void Nexus::ScriptEngine::LoadCoreAssemblyClasses()
{
    m_EntityClasses.clear();

    const MonoTableInfo* table = mono_image_get_table_info(m_CoreAssemblyImage, MONO_TABLE_TYPEDEF);
    int32_t count = mono_table_info_get_rows(table);

    m_EntityBaseClass = mono_class_from_name(m_CoreAssemblyImage, "Nexus", "Entity");
    m_EntityBaseConstructor = mono_class_get_method_from_name(m_EntityBaseClass, ".ctor", 1);

    NEXUS_LOG_WARN("Loading Script Classes...");
    for (int32_t i = 0; i < count; i++)
    {
        uint32_t cols[MONO_TYPEDEF_SIZE];
        mono_metadata_decode_row(table, i, cols, MONO_TYPEDEF_SIZE);

        const char* nameSpace = mono_metadata_string_heap(m_CoreAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
        const char* name = mono_metadata_string_heap(m_CoreAssemblyImage, cols[MONO_TYPEDEF_NAME]);

        std::string Fullname;
        if (strlen(nameSpace) != 0)
            Fullname = fmt::format("{}.{}", nameSpace, name);
        else
            Fullname = name;

        MonoClass* cl = mono_class_from_name(m_CoreAssemblyImage, nameSpace, name);

        if (cl == m_EntityBaseClass)
            continue;

        if (mono_class_is_subclass_of(cl, m_EntityBaseClass, false))
        {
            std::string key = nameSpace + std::string(".") + name;

            m_EntityClasses[key] = cl;
            NEXUS_LOG_TRACE("Loaded: {0}", key);
        }
    }
    NEXUS_LOG_WARN("");
}

char* Nexus::ScriptEngine::ReadBytes(const std::string& filepath, uint32_t* outSize)
{
    std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

    if (!stream)
    {
        // Failed to open the file
        return nullptr;
    }

    std::streampos end = stream.tellg();
    stream.seekg(0, std::ios::beg);
    uint32_t size = end - stream.tellg();

    if (size == 0)
    {
        // File is empty
        return nullptr;
    }

    char* buffer = new char[size];
    stream.read((char*)buffer, size);
    stream.close();

    *outSize = size;
    return buffer;
}

MonoAssembly* Nexus::ScriptEngine::LoadAssembly(const std::string& assemblyPath)
{
    uint32_t fileSize = 0;
    char* fileData = ReadBytes(assemblyPath, &fileSize);

    // NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
    MonoImageOpenStatus status;
    MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

    if (status != MONO_IMAGE_OK)
    {
        const char* errorMessage = mono_image_strerror(status);
        // Log some error message using the errorMessage data
        return nullptr;
    }

    MonoAssembly* assembly = mono_assembly_load_from_full(image, assemblyPath.c_str(), &status, 0);
    mono_image_close(image);

    // Don't forget to free the file data
    delete[] fileData;

    return assembly;
}

Nexus::ScriptInstance::ScriptInstance(const std::string& ClassName)
{
    m_Class = ScriptEngine::s_Instance->m_EntityClasses[ClassName];
    m_Object = mono_object_new(ScriptEngine::s_Instance->m_AppDomain, m_Class);
    mono_runtime_object_init(m_Object);
    
    m_OnCreate = mono_class_get_method_from_name(m_Class, "OnCreate", 0);
    m_OnUpdate = mono_class_get_method_from_name(m_Class, "OnUpdate", 1);
    m_OnDestroy = mono_class_get_method_from_name(m_Class, "OnDestroy", 0);
}

void Nexus::ScriptInstance::InVokeOnCreate()
{
    mono_runtime_invoke(m_OnCreate, m_Object, nullptr, nullptr);
}

void Nexus::ScriptInstance::InVokeOnUpdate(float ts)
{
    void* param = &ts;
    mono_runtime_invoke(m_OnUpdate, m_Object, &param, nullptr);
}

void Nexus::ScriptInstance::InVokeOnDestroy()
{
    mono_runtime_invoke(m_OnDestroy, m_Object, nullptr, nullptr);
}
