#pragma once
#include "ProjectSpecification.h"

namespace Nexus
{
	class ProjectSerializer
	{
	public:
		static bool Serialize(const ProjectSpecifications& specs);
		static bool DeSerialize(const std::string& path,ProjectSpecifications& specs);
	};
}
