#include "nxpch.h"
#include "Component.h"
#include "Assets/AssetManager.h"

Nexus::Component::Identity::Identity(uint64_t id)
	:Id(id)
{}

Nexus::Component::Transform::Transform(glm::vec3 translation)
	:Translation(translation), Rotation(glm::vec3(0.f)), Scale(glm::vec3(1.f))
{}

Nexus::Component::Mesh::Mesh(AssetHandle assetHandle)
	:handle(assetHandle)
{}