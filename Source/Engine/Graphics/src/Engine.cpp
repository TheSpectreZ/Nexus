#include "Graphics/Engine.h"
#include "Graphics/Presenter.h"

#include "Backend.h"

void Nexus::Graphics::Engine::Initialize(const EngineSpecification& specs)
{
	Backend::Init(specs);
	Presenter::Init(specs);
}

void Nexus::Graphics::Engine::Shutdown()
{
	Presenter::Shut();
	Backend::Shut();
}
