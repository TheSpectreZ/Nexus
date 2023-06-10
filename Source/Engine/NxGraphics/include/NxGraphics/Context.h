#pragma once
#include "NxCore/Base.h"
#include "TypeImpls.h"

namespace Nexus
{
	struct ContextCreateInfo
	{
		void* hwnd;
		void* hInstance;
	};

	class Context
	{
	public:
		virtual ~Context() = default;

		virtual void Initialize() = 0;
		virtual void Shutdown() = 0;
		virtual void WaitForDevice() = 0;
	};
}