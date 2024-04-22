 #pragma once
#include <queue>
#include <typeindex>
#include <unordered_map>
#include <functional>
#include <memory>

#ifdef NEXUS_CORE_SHARED_BUILD
#define NEXUS_CORE_API __declspec(dllexport)
#else 
#define NEXUS_CORE_API __declspec(dllimport)
#endif // NEXUS_CORE_SHARED_BUILD

namespace Nexus
{
	struct Event
	{
		virtual ~Event() {}
	};

	class NEXUS_CORE_API EventDispatcher
	{
	public:
		template<typename T> requires std::derived_from<T, Event>
		void AddListener(std::function<void(T)> func)
		{
			std::type_index type = typeid(T);
			mEventFuncs[type].push_back(
				[func](std::shared_ptr<Event> baseEvent)
				{
					func(*(T*)baseEvent.get());
				});
		}

		template<typename T, typename... Args> requires std::derived_from<T, Event>
		void Trigger(Args&&... args)
		{
			std::shared_ptr<Event> baseEvent = std::make_shared<T>(std::forward<Args>(args)...);
			mEventQueue[typeid(T)].push(baseEvent);
		}

		void Dispatch();
	private:
		using baseEvent = std::shared_ptr<Event>;
		using baseFunc  = std::function<void(baseEvent)>;

		template<typename Key, typename Value>
		using map = std::unordered_map<Key, Value>;

		map<std::type_index, std::queue<baseEvent> > mEventQueue;
		map<std::type_index, std::vector<baseFunc> > mEventFuncs;
	};
}