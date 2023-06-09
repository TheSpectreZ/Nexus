#pragma once

namespace Nexus
{
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
		
		bool Vsync = true;
		bool MultiSampling = false;
		bool EnableScriptEngine = false;
		bool LoadDefaultAssets = false;

		PhysicsAPI pApi;
	};
}