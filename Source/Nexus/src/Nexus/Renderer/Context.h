#pragma once

namespace Nexus
{
	class Context
	{
	public:
		static Ref<Context> Create();

		Context() = default;
		~Context() = default;

		virtual void Init() = 0;
		virtual void Shut() = 0;
		virtual void WaitForDevice() = 0;
	};
}