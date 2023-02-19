#include "Utility/Registry.h"
#include "Utility/Entity.h"

void Nexus::Utility::Registry::Clear()
{
	m_registry.clear();
}

Nexus::Utility::Entity Nexus::Utility::Registry::CreateEntity()
{
	return  Entity(m_registry.create(), &m_registry);
}

void Nexus::Utility::Registry::DestroyEntity(Entity entity)
{
	m_registry.destroy(entity);
}
