#pragma once

namespace Nexus
{
	class Asset
	{
		friend class AssetManager;
	public:
		Asset() = default;
		virtual ~Asset() = default;

		UUID GetID() { return m_Id; }
	protected:
		UUID m_Id = NullUUID;
	};
}