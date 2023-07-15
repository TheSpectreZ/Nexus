#pragma once

namespace Nexus
{
	struct Window
	{
		int width, height;
		const char* title;
		void* glfwHandle;
		void* nativeHandle;
	};
}