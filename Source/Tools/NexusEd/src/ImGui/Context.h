#pragma once

namespace NexusEd
{
	class Context
	{
		static Context* s_Instance;
	public:
		static Context* Get() { return s_Instance; }
		static void Initialize();
		static void Shutdown();
		static void OnWindowResize();

		void BeginFrame();
		void EndFrame();
	private:
		void ImplFonts();
		void ImplStyles();

		void ImplWindowInit();
		void ImplWindowShut();

		void ImplVulkanInit();
		void ImplVulkanShut();
	};
}
