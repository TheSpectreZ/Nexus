#pragma once
#include "TimeStep.h"

#ifdef NEXUS_CORE_SHARED_BUILD
#define NEXUS_CORE_API __declspec(dllexport)
#else 
#define NEXUS_CORE_API __declspec(dllimport)
#endif // NEXUS_CORE_SHARED_BUILD

namespace Nexus
{
	class NEXUS_CORE_API Layer
	{
	public:
		virtual void OnAttach(){};
  		virtual void OnUpdate(Timestep ts){};
		virtual void OnRender(){};
		virtual void OnDetach(){};
		virtual void OnWindowResize(int width, int height) {};
		virtual ~Layer() {};
	};
}
