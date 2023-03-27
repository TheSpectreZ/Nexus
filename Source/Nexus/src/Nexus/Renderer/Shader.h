#pragma once
#include "RenderTypes.h"
#include <filesystem>

typedef std::vector<uint32_t> SpirV;

namespace Nexus
{
	class Shader
	{
	public:		
		static Ref<Shader> Create(const std::string& Filepath);
		virtual void Destroy() = 0;

		Shader() = default;
		~Shader() = default;
	};

	class ShaderLib
	{
		static ShaderLib* s_Instance;
	public:
		static void Initialize();
		static void Terminate();

		static Ref<Shader> Get(const std::string& shaderPath);
	private:
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;
	};
}
