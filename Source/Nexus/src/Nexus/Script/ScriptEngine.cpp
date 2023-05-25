#include "nxpch.h"
#include "ScriptEngine.h"
#include "ScriptGlue.h"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"
#include "mono/metadata/attrdefs.h"

#include "Scene/Entity.h"
#include "Core/Application.h"
#include "FileWatch.hpp"

Nexus::ScriptEngine* Nexus::ScriptEngine::s_Instance = nullptr;

static Nexus::Scope<filewatch::FileWatch<std::string>> s_ScriptEngineFilewatcher;

void Nexus::ScriptEngine::Init()
{
    s_Instance = new ScriptEngine();
	s_Instance->InitMono();
    ScriptGlue::BindInternalCalls();
}

void Nexus::ScriptEngine::Shut()
{
    s_Instance->ShutdownMono();

    s_ScriptEngineFilewatcher.reset();
    delete s_Instance;
}

void Nexus::ScriptEngine::SetAppAssemblyFilepath(const std::string& filepath)
{
    s_Instance->m_AppAsseblyPath = filepath;

    s_ScriptEngineFilewatcher.reset();

    s_ScriptEngineFilewatcher = CreateScope<filewatch::FileWatch<std::string>>(filepath, [&](const std::string& path,const filewatch::Event eventType)
        {
            NEXUS_LOG_DEBUG("Watched File: {0} - {1}", path, (uint32_t)eventType);
            
            if (!s_Instance->m_AssemblyReloadPending && eventType == filewatch::Event::added || eventType == filewatch::Event::modified)
            {    
                s_Instance->m_AssemblyReloadPending = true;

                using namespace std::chrono_literals;
                std::this_thread::sleep_for(500ms);

                Application::Get()->SubmitToMainThreadQueue([]() {ScriptEngine::ReloadAssembly(); });
            }
        });

    if (std::filesystem::exists(filepath))
        ReloadAssembly();
}

void Nexus::ScriptEngine::ReloadAssembly()
{
    if (s_Instance->m_AppDomain)
    {
        mono_domain_set(mono_get_root_domain(), false);
        mono_domain_unload(s_Instance->m_AppDomain);
    }

    s_Instance->InitAssembly();
    ScriptGlue::BindComponents();
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

        Nexus::ScriptInstance inst(&s_Instance->m_EntityClasses[Script.name]);

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
}

void Nexus::ScriptEngine::ShutdownMono()
{
    mono_domain_set(mono_get_root_domain(), false);
    mono_domain_unload(s_Instance->m_AppDomain);

    mono_jit_cleanup(m_RootDomain);
    m_RootDomain = nullptr;
}

void Nexus::ScriptEngine::InitAssembly()
{
    char friendlyName[] = "NexusAppDomain";
    m_AppDomain = mono_domain_create_appdomain(friendlyName, nullptr);
    mono_domain_set(m_AppDomain, true);

    m_CoreAssembly = LoadAssembly("Resources/Scripts/NexusScriptCore.dll");
    m_CoreAssemblyImage = mono_assembly_get_image(m_CoreAssembly);

    PrintAssemblyTypes(m_CoreAssembly, "Core Assembly");
    
    m_AppAssembly = LoadAssembly(m_AppAsseblyPath);
    m_AppAssemblyImage = mono_assembly_get_image(m_AppAssembly);

    PrintAssemblyTypes(m_AppAssembly, "App Assembly");

    LoadAppAssemblyClasses();
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

void Nexus::ScriptEngine::LoadAppAssemblyClasses()
{
    m_EntityClasses.clear();

    const MonoTableInfo* table = mono_image_get_table_info(m_AppAssemblyImage, MONO_TABLE_TYPEDEF);
    int32_t count = mono_table_info_get_rows(table);

    m_EntityBaseClass = mono_class_from_name(m_CoreAssemblyImage, "Nexus", "Entity");
    m_EntityBaseConstructor = mono_class_get_method_from_name(m_EntityBaseClass, ".ctor", 1);

    NEXUS_LOG_WARN("Loading Script Classes...");
    for (int32_t i = 0; i < count; i++)
    {
        uint32_t cols[MONO_TYPEDEF_SIZE];
        mono_metadata_decode_row(table, i, cols, MONO_TYPEDEF_SIZE);

        const char* nameSpace = mono_metadata_string_heap(m_AppAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
        const char* name = mono_metadata_string_heap(m_AppAssemblyImage, cols[MONO_TYPEDEF_NAME]);

        std::string Fullname;
        if (strlen(nameSpace) != 0)
            Fullname = fmt::format("{}.{}", nameSpace, name);
        else
            Fullname = name;

        MonoClass* cl = mono_class_from_name(m_AppAssemblyImage, nameSpace, name);

        if (cl == m_EntityBaseClass)
            continue;

        if (mono_class_is_subclass_of(cl, m_EntityBaseClass, false))
        {
            std::string key = nameSpace + std::string(".") + name;

            m_EntityClasses[key] = ScriptClass(cl);
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
    uint32_t size = (uint32_t)(end - stream.tellg());

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

Nexus::ScriptInstance::ScriptInstance(ScriptClass* Class)
{
    m_ScriptClass = Class;
    m_Object = mono_object_new(ScriptEngine::s_Instance->m_AppDomain, m_ScriptClass->m_Class);
    mono_runtime_object_init(m_Object);
}

void Nexus::ScriptInstance::InVokeOnCreate()
{
    mono_runtime_invoke(m_ScriptClass->m_OnCreate, m_Object, nullptr, nullptr);
}

void Nexus::ScriptInstance::InVokeOnUpdate(float ts)
{
    void* param = &ts;
    mono_runtime_invoke(m_ScriptClass->m_OnUpdate, m_Object, &param, nullptr);
}

void Nexus::ScriptInstance::InVokeOnDestroy()
{
    mono_runtime_invoke(m_ScriptClass->m_OnDestroy, m_Object, nullptr, nullptr);
}

bool Nexus::ScriptInstance::GetFieldValueInternal(const std::string& name,void* buffer)
{
    if (!m_ScriptClass->m_Fields.contains(name))
        return false;

    ScriptField& f = m_ScriptClass->m_Fields[name];
    mono_field_get_value(m_Object, f.Field, buffer);
    return true;
}

void Nexus::ScriptInstance::SetFieldValue(const std::string& name,void* value)
{
    if (!m_ScriptClass->m_Fields.contains(name))
        return;

    ScriptField& f = m_ScriptClass->m_Fields[name];
    mono_field_set_value(m_Object, f.Field, value);
}

Nexus::ScriptFieldType GetScriptFieldType(MonoType* type)
{
    std::string Typename = mono_type_get_name_full(type, MonoTypeNameFormat::MONO_TYPE_NAME_FORMAT_FULL_NAME);
    
    if (Typename == "System.Single")
        return Nexus::ScriptFieldType::Float;
    if (Typename == "System.Double")
        return Nexus::ScriptFieldType::Double;
    if (Typename == "Nexus.Vector3")
        return Nexus::ScriptFieldType::Vec3;

    return Nexus::ScriptFieldType::None;
}

const char* GetScriptFieldTypeString(Nexus::ScriptFieldType Type)
{
    switch (Type)
    {
    case Nexus::ScriptFieldType::None:
        return "None";  
    case Nexus::ScriptFieldType::Float:
        return "Float";
    case Nexus::ScriptFieldType::Double:
        return "Double";
    case Nexus::ScriptFieldType::UInt:
        return "Unsigned Integer";
    case Nexus::ScriptFieldType::Int:
        return "Integer";
    case Nexus::ScriptFieldType::Bool:
        return "Bool";
    case Nexus::ScriptFieldType::Vec2:
        return "Vector2";  
    case Nexus::ScriptFieldType::Vec3:
        return "Vector3";
    case Nexus::ScriptFieldType::Vec4:
        return "Vector4";
    default:
        return "<InValid>";
    }
}

Nexus::ScriptClass::ScriptClass(MonoClass* Class)
{
    m_Class = Class;

    m_OnCreate = mono_class_get_method_from_name(m_Class, "OnCreate", 0);
    m_OnUpdate = mono_class_get_method_from_name(m_Class, "OnUpdate", 1);
    m_OnDestroy = mono_class_get_method_from_name(m_Class, "OnDestroy", 0);

    void* Iter = nullptr;
    while (MonoClassField* field = mono_class_get_fields(m_Class, &Iter))
    {
        uint32_t flag = mono_field_get_flags(field);

        if (flag & MONO_FIELD_ATTR_PUBLIC)
        {
            std::string name = mono_field_get_name(field);
            ScriptFieldType type = GetScriptFieldType(mono_field_get_type(field));
            m_Fields[name] = { type ,field };

            NEXUS_LOG_TRACE("\t{0} : {1}", name, GetScriptFieldTypeString(type));
        }

    }
}
