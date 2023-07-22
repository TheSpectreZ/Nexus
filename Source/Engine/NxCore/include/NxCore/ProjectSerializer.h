#pragma once
#include "ProjectSpecification.h"

namespace Nexus
{
	class NEXUS_CORE_API ProjectSerializer
	{
	public:
		static bool Serialize(const ProjectSpecifications& specs);
		static bool DeSerialize(const std::string& path,ProjectSpecifications& specs);
	};
}
