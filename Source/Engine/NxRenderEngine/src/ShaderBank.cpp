#include "NxRenderEngine/ShaderBank.h"
#include "NxCore/Logger.h"

Nexus::Ref<Nexus::Shader> Nexus::ShaderBank::Load(const std::string& path)
{
    auto specs = ShaderCompiler::CompileFromFile(path);
    m_Shaders[path] = GraphicsInterface::CreateShader(specs);

    NEXUS_LOG("Renderer", "Shader Created: %s\n", path.c_str());

    return m_Shaders[path];
}

Nexus::Ref<Nexus::Shader> Nexus::ShaderBank::Get(const std::string& path)
{
    return m_Shaders[path];
}
