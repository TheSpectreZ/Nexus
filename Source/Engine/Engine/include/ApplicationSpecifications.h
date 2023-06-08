#pragma once

namespace Nexus
{
	enum RenderApi
	{
		RenderAPI_None,
		RenderAPI_Vulkan
	};

	enum PhysicsAPI
	{
		PhysicsAPI_None,
		PhysicsAPI_Jolt
	};

	struct ApplicationSpecifications
	{
		int Window_Width;
		int Window_height;
		const char* Window_Title;
		
		bool Vsync;
		bool MultiSampling;
		bool EnableScriptEngine;
		bool LoadDefaultAssets;

		RenderApi rApi;
		PhysicsAPI pApi;
	};
}