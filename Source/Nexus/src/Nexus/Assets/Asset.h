#pragma once

namespace Nexus
{
	class Asset
	{
	public:
		Asset();
		virtual ~Asset() = default;

		UUID GetID() { return m_Id; }
	protected:
		UUID m_Id;
	};
}