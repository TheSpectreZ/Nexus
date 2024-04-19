#include "NxGameEngine/Layer.h"
#include "NxGameEngine/Engine.h"

Nexus::GraphicsDevice* Nexus::ILayer::getGraphicsDevice()
{
	return mEngine->mRenderEngine.getGraphicsDevice();
}
