#include "NxRenderEngine/RenderSettings.h"

Nexus::ResolutionChangeEvent::ResolutionChangeEvent(Resolution resolution)
{
	switch (resolution)
	{
	case Nexus::Resolution::e4K:
		width = 3840; 
		height = 2160;
		break;
	case Nexus::Resolution::e1440:
		width = 2560;
		height = 1440;
		break;
	case Nexus::Resolution::e1080:
		width = 1920;
		height = 1080;
		break;
	default:
		width = 1280;
		height = 720;
	}
}