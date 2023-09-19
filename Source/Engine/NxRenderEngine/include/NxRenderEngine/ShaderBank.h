#pragma once
#include "GraphicsInterface.h"

namespace Nexus
{
	class ShaderBank
	{
	public:
		ShaderBank() = default;
		~ShaderBank() = default;

		Ref<Shader> Load(const std::string& path);
		Ref<Shader> Get(const std::string& path);
	private:
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;
	};
}
