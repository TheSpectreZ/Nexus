#pragma once
#include "imgui.h"

namespace Nexus
{
	class EditorContext
	{
	public:
		EditorContext() = default;
		virtual ~EditorContext() = default;

		static void Initialize();
		static void Shutdown();

		static void StartFrame();
		static void Render();
	private:
		virtual void Start() = 0;
		virtual void End() = 0;
		
		static EditorContext* s_Instance;
	};
}