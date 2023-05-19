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
		std::string GetPath() { return m_Path; }
	protected:
		UUID m_Id = NullUUID;
		std::string m_Path;
	};
}