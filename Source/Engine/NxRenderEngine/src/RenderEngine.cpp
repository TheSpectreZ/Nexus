#include "RenderEngine.h"

Nexus::RenderEngine* Nexus::RenderEngine::s_Instance = nullptr;
static Nexus::RenderAPIType s_Type = Nexus::RenderAPIType::NONE;

void Nexus::RenderEngine::Initialize(RenderAPIType Type)
{
	s_Instance = new RenderEngine;
	s_Type = Type;
}

void Nexus::RenderEngine::Shutdown()
{
	s_Type = RenderAPIType::NONE;
	delete s_Instance;
}
