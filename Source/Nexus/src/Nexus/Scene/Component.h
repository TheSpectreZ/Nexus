#pragma once
#include "Assets/Asset.h"
#include "glm/glm.hpp"

namespace Nexus::Component
{
	struct Identity
	{
		Identity() = default;
		Identity(const Identity&) = default;
		Identity(uint64_t id);

		uint64_t Id;
	};

	struct Transform
	{
		Transform() = default;
		Transform(const Transform&) = default;
		Transform(const glm::vec3 translation);

		glm::vec3 Translation;
		glm::vec3 Rotation;
		glm::vec3 Scale;
	};

	struct Mesh
	{
		Mesh() = default;
		Mesh(const Mesh&) = default;
		Mesh(AssetHandle assetHandle);

		AssetHandle handle;
	};

}