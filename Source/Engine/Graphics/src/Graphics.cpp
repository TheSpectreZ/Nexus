#include "Graphics/Graphics.h"
#include "Graphics/Backend.h"

void Nexus::Graphics::Engine::Initialize()
{
	Backend::Get().Init();
}

void Nexus::Graphics::Engine::Render()
{
}

void Nexus::Graphics::Engine::Shutdown()
{
	Backend::Get().Shut();
}
