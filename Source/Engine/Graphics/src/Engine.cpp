#include "Graphics/Engine.h"
#include "Graphics/Backend.h"
#include "Graphics/Presenter.h"

void Nexus::Graphics::Engine::Initialize(const EngineSpecification& specs)
{
	Backend::Init(specs);
	Presenter::Init(specs);
}

void Nexus::Graphics::Engine::Render()
{
	Presenter::StartFrame();

	Presenter::EndFrame();
}

void Nexus::Graphics::Engine::Shutdown()
{
	Presenter::Shut();
	Backend::Shut();
}
