#pragma once

typedef struct GLFWwindow GLFWwindow;

namespace Nexus
{
	struct Window
	{
		int width, height;
		const char* title;
		GLFWwindow* handle;
	};
}