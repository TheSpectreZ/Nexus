#pragma once
#include <memory>
#include <functional>
#include <type_traits>

#ifdef NEXUS_CORE_SHARED_BUILD
#define NEXUS_CORE_API __declspec(dllexport)
#else 
#define NEXUS_CORE_API __declspec(dllimport)
#endif // NEXUS_CORE_SHARED_BUILD

#define ENUM_FLAG_OPERATORS(T)                                                                                                                                            \
    inline T NEXUS_CORE_API operator~ (T a) { return static_cast<T>( ~static_cast<std::underlying_type<T>::type>(a) ); }                                                                       \
    inline T NEXUS_CORE_API operator| (T a, T b) { return static_cast<T>( static_cast<std::underlying_type<T>::type>(a) | static_cast<std::underlying_type<T>::type>(b) ); }                   \
    inline T NEXUS_CORE_API operator& (T a, T b) { return static_cast<T>( static_cast<std::underlying_type<T>::type>(a) & static_cast<std::underlying_type<T>::type>(b) ); }                   \
    inline T NEXUS_CORE_API operator^ (T a, T b) { return static_cast<T>( static_cast<std::underlying_type<T>::type>(a) ^ static_cast<std::underlying_type<T>::type>(b) ); }                   \
    inline T& NEXUS_CORE_API operator|= (T& a, T b) { return reinterpret_cast<T&>( reinterpret_cast<std::underlying_type<T>::type&>(a) |= static_cast<std::underlying_type<T>::type>(b) ); }   \
    inline T& NEXUS_CORE_API operator&= (T& a, T b) { return reinterpret_cast<T&>( reinterpret_cast<std::underlying_type<T>::type&>(a) &= static_cast<std::underlying_type<T>::type>(b) ); }   \
    inline T& NEXUS_CORE_API operator^= (T& a, T b) { return reinterpret_cast<T&>( reinterpret_cast<std::underlying_type<T>::type&>(a) ^= static_cast<std::underlying_type<T>::type>(b) ); }

namespace Nexus
{
	template<typename T>
	bool NEXUS_CORE_API IsFlagSet(T flag, T value)
	{
		return (flag & value) == value;
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T,typename... Args>
	Ref<T> NEXUS_CORE_API CreateRef(Args&&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template<typename T,typename... Args>
	Scope<T> NEXUS_CORE_API CreateScope(Args&&... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T1,typename T2>
	Ref<T1> NEXUS_CORE_API DynamicPointerCast(Ref<T2> other)
	{
		return std::dynamic_pointer_cast<T1>(other);
	}

}

#define NEXUS_BIND_FN(fn,ref) [ref](auto&&... args) -> decltype(auto) { return ref->fn(std::forward<decltype(args)>(args)...); }
