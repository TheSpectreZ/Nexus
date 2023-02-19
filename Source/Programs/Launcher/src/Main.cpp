#define WINDOWED
#include "Application/EntryPoint.h"

class Launcher : public Nexus::Application
{
public:
	Launcher()
	{
		
	}
};

Nexus::Application* CreateApplication()
{
	return new Launcher();
}