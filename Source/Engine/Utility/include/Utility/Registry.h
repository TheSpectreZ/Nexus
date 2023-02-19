#pragma once
#include "Build.h"
#include "entt/entt.hpp"

namespace Nexus::Utility
{
	class Entity;

	class Registry
	{
	public:
		void Clear();

		Entity CreateEntity();
		void DestroyEntity(Entity entity);
	private:
		entt::registry m_registry;
	};
}