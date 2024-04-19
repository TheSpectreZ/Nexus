#include "NxShaderCompiler/Compiler.h"
#include "NxCore/Logger.h"
#include "NxCore/StringUtils.h"
#include "NxCore/Platform.h"

#include <fstream>
#include <sstream>
#include <filesystem>

namespace Nexus::ShaderCompiler::Utils
{
    static const char* ReturnTargetString(ShaderType inType)
    {
        switch (inType)
        {
        case ShaderType::eFragment:
            return "frag";
        case ShaderType::eVertex:
            return "vert";
        case ShaderType::eCompute:
            return "comp";
        default:
            return "none";
        }
    }
}

bool Nexus::ShaderCompiler::CompileFromFile(const char* filepath, ShaderType type, ShaderSpecification* pOutSpecs)
{
    std::filesystem::path path = filepath;
    auto name = path.filename().stem().generic_string() + ".spirv";
    auto cachedPath = path.parent_path() / name;

    auto cachedPathString = cachedPath.generic_string();
    //if (std::filesystem::exists(cachedPathString))
    //{
    //    NEXUS_LOG("ShaderCompiler", "ShaderCache Found: %s", cachedPathString.c_str());
    //
    //    std::ifstream file(cachedPathString, std::ios::binary | std::ios::ate);
    //    if (!file.is_open())
    //    {
    //        NEXUS_LOG("ShaderCompiler", "Unable to Open File: %s", cachedPathString.c_str());
    //        return false;
    //    }
    //
    //    auto size = file.tellg();
    //    pInBlob->spirv.resize(size / sizeof(uint32_t));
    //
    //    if (!file.read((char*)pInBlob->spirv.data(), size))
    //    {
    //        NEXUS_LOG("ShaderCompiler", "Error Reading from file: %s", cachedPathString.c_str());
    //        return false;
    //    }
    //
    //    pInBlob->type = type;
    //    return true;
    //}


    auto glslPath = path.generic_string();
    
    std::ifstream file(glslPath);
    if (!file.is_open())
    {
        NEXUS_LOG("ShaderCompiler", "Unable to Open File: %s", glslPath.c_str());
        return false;
    }

    std::stringstream ss;
    ss << file.rdbuf();

    auto Binpath = (std::filesystem::current_path() / "Assets/Tools/glslang.exe").generic_string();
    auto CommandString = format_string("\"%s\" -V --target-env vulkan1.3 -g -S %s -e %s -o \"%s\"",
        filepath,
        Utils::ReturnTargetString(type),
        "main",    
        cachedPathString.c_str()
    );

    Platform::LaunchProcess(Binpath + " " + CommandString);

    file = std::ifstream(cachedPathString, std::ifstream::binary | std::ifstream::ate);

    if (!file.is_open())
    {
        NEXUS_LOG("ShaderCompiler", "Unable to Open File: %s", cachedPathString.c_str());
        return false;
    }

    auto fileSize = file.tellg();
    pOutSpecs->spirv.resize(fileSize);

    file.seekg(0, std::ios::beg);
    file.read((char*)pOutSpecs->spirv.data(), fileSize);
    
    pOutSpecs->type = type;
    return true;
}
