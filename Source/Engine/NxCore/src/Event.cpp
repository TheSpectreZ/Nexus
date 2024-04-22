#include "NxCore/Event.h"

void Nexus::EventDispatcher::Dispatch()
{
	for (auto& [type, queue] : mEventQueue)
	{
		while (!queue.empty())
		{
			if (mEventFuncs.contains(type))
			{
				auto& event = queue.front();
				for (auto& func : mEventFuncs[type])
				{
					func(event);
				}
			}
			
			queue.pop();
		}
	}
}
