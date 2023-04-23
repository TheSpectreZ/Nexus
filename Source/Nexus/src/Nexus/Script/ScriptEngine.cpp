#include "nxpch.h"
#include "ScriptEngine.h"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"

namespace Nexus
{
    /// 
    /// Script Engine Data
    /// 

    struct ScriptEngineData
    {
        MonoDomain* rootDomain = nullptr;
        MonoDomain* appDomain = nullptr;

        MonoAssembly* coreAssembly = nullptr;
    };

    static ScriptEngineData s_ScriptEngineData;

    /// 
    /// Helper Funtions
    /// 

    char* ReadBytes(const std::string& filepath, uint32_t* outSize)
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

    MonoAssembly* LoadCSharpAssembly(const std::string& assemblyPath)
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

    void PrintAssemblyTypes(MonoAssembly* assembly)
    {
        MonoImage* image = mono_assembly_get_image(assembly);
        const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
        int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

        std::cout << std::endl;
        for (int32_t i = 0; i < numTypes; i++)
        {
            uint32_t cols[MONO_TYPEDEF_SIZE];
            mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

            const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
            const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

            NEXUS_LOG_TRACE("{0}.{1}", nameSpace, name);
        }
    }

}


void Nexus::ScriptEngine::Init()
{
	InitMono();
}

void Nexus::ScriptEngine::Shut()
{
    ShutdownMono();
}

void Nexus::ScriptEngine::InitMono()
{
	mono_set_assemblies_path("mono/lib");

	MonoDomain* rootDomain = mono_jit_init("NexusJITRuntime");
	NEXUS_ASSERT((rootDomain == nullptr), "Failed To Initialize Mono JIT");
	s_ScriptEngineData.rootDomain = rootDomain;

	char friendlyName[] = "NexusAppDomain";
	s_ScriptEngineData.appDomain = mono_domain_create_appdomain(friendlyName, nullptr);
    mono_domain_set(s_ScriptEngineData.appDomain, true);

    s_ScriptEngineData.coreAssembly = LoadCSharpAssembly("Resources/Scripts/NexusScriptCore.dll");
    PrintAssemblyTypes(s_ScriptEngineData.coreAssembly);

    MonoImage* image = mono_assembly_get_image(s_ScriptEngineData.coreAssembly);
    MonoClass* monoClass = mono_class_from_name(image, "Nexus", "Main");

    MonoObject* instance = mono_object_new(s_ScriptEngineData.appDomain, monoClass);
    mono_runtime_object_init(instance);

    {
        MonoMethod* printMessageFnc = mono_class_get_method_from_name(monoClass, "PrintMessage", 0);
        mono_runtime_invoke(printMessageFnc, instance, nullptr, nullptr);
    }

    {
        MonoMethod* printIntFnc = mono_class_get_method_from_name(monoClass, "PrintInt", 1);

        int value = 5;
        void* IntParam = &value;
        mono_runtime_invoke(printIntFnc, instance, &IntParam, nullptr);
    }

    {
        MonoString* monostring = mono_string_new(s_ScriptEngineData.appDomain, "Hello World From C++");

        MonoMethod* printCustomMessageFnc = mono_class_get_method_from_name(monoClass, "PrintCustomMessage", 1);
        
        void* stringparam = monostring;
        mono_runtime_invoke(printCustomMessageFnc, instance, &stringparam, nullptr);
    }
}

void Nexus::ScriptEngine::ShutdownMono()
{
    s_ScriptEngineData.appDomain = nullptr;

    mono_jit_cleanup(s_ScriptEngineData.rootDomain);
    s_ScriptEngineData.rootDomain = nullptr;
}

