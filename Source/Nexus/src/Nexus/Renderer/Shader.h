#pragma once

typedef std::vector<uint32_t> SpirV;

namespace Nexus
{
	class Shader
	{
	public:		
		static Ref<Shader> Create(const std::string& Filepath);

		Shader() = default;
		virtual ~Shader() = default;
	};
}
