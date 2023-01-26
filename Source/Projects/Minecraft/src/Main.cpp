#include "Core/EntryPoint.h"
#include "Minecraft.h"

class MinecraftApplication : public Nexus::Application
{
public:
	
	MinecraftApplication()
	{
		p_Window.handle = nullptr;
		p_Window.width = 1280;
		p_Window.height = 720;
		p_Window.title = "Minecraft";

		PushLayer(new Minecraft());
	}

	~MinecraftApplication()
	{

	}

};

Nexus::Application* CreateApplication()
{
	return new MinecraftApplication();
}