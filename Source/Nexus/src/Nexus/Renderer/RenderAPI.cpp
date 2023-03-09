#include "nxpch.h"
#include "RenderAPI.h"

Nexus::RenderAPIType Nexus::RenderAPI::s_Type = Nexus::RenderAPIType::NONE;

Nexus::RenderAPIType Nexus::RenderAPI::GetCurrentAPI()
{
	return s_Type;
}

void Nexus::RenderAPI::SetRenderAPI(RenderAPIType Type)
{
	s_Type = Type;
}
