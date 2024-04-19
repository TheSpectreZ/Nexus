#pragma once
#include <vector>

namespace Nexus
{
	enum class ShaderType
	{
		eVertex,
		eFragment,
		eCompute
	};

	struct ShaderSpecification
	{
		ShaderType type;
		std::vector<uint8_t> spirv;
	};

	class ShaderObject
	{
	public:
		virtual ~ShaderObject() = default;
	};
}