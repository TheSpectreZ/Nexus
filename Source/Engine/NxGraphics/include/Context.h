#pragma once

namespace Nexus
{
	class Context
	{
	public:
		virtual void Initialize() = 0;
		virtual void Shutdown() = 0;
		virtual void WaitForDevice() = 0;
	};
}