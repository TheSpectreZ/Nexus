#pragma once
#include "NxRenderEngine/RendererAPI.h"

namespace Nexus
{
	struct ApplicationSpecifications
	{
		int Window_Width;
		int Window_height;
		const char* Window_Title;
		
		bool Vsync = true;
		bool MultiSampling = false;
		bool EnableScriptEngine = false;
		bool EnableRendererSubmodules = false;

		RendererAPI rApi;
	};
}