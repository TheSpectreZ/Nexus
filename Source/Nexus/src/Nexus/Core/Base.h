#pragma once
#include <memory>

namespace Nexus
{
	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T,typename... Args>
	Ref<T> CreateRef(Args&&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template<typename T,typename... Args>
	Scope<T> CreateScope(Args&&... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T1,typename T2>
	Ref<T1> DynamicPointerCast(Ref<T2> other)
	{
		return std::dynamic_pointer_cast<T1>(other);
	}

}