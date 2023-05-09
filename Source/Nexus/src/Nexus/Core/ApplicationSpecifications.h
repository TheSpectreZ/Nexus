#pragma once

namespace Nexus
{
	enum RenderApi
	{
		RenderAPI_Vulkan
	};

	enum PhysicsAPI
	{
		PhysicsAPI_Jolt
	};

	struct ApplicationSpecifications
	{
		int Window_Width;
		int Window_height;
		const char* Window_Title;
		bool Vsync;
		bool MultiSampling;

		RenderApi rApi;
		PhysicsAPI pApi;
	};
}