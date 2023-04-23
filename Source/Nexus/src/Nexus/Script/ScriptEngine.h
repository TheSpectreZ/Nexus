#pragma once

namespace Nexus
{
	class ScriptEngine
	{
	public:
		static void Init();
		static void Shut();
	private:
		static void InitMono();
		static void ShutdownMono();
	};
}
