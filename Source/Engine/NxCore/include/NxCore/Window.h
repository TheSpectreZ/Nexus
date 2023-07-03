#pragma once

namespace Nexus
{
	struct Window
	{
		int width, height;
		const char* title;
		void* handle;
		void* nativeHandle;
	};
}