#pragma once
#include "spdlog/spdlog.h"
#include <memory>

#ifdef NEXUS_CORE_SHARED_BUILD
#define NEXUS_CORE_API __declspec(dllexport)
#else 
#define NEXUS_CORE_API __declspec(dllimport)
#endif // NEXUS_CORE_SHARED_BUILD

namespace Nexus::Utility
{
	struct NEXUS_CORE_API Logger
	{
		static void Init();
		static void Shut();

		static std::shared_ptr<spdlog::logger> _logger;
	};
}

#ifdef NEXUS_ENABLE_LOG

#define NEXUS_LOG_INIT ::Nexus::Utility::Logger::Init();
#define NEXUS_LOG_SHUT ::Nexus::Utility::Logger::Shut();

#define NEXUS_LOG_DEBUG(...) ::Nexus::Utility::Logger::_logger->debug(__VA_ARGS__);
#define NEXUS_LOG_TRACE(...) ::Nexus::Utility::Logger::_logger->trace(__VA_ARGS__);
#define NEXUS_LOG_INFO(...) ::Nexus::Utility::Logger::_logger->info(__VA_ARGS__);
#define NEXUS_LOG_WARN(...) ::Nexus::Utility::Logger::_logger->warn(__VA_ARGS__);
#define NEXUS_LOG_ERROR(...) ::Nexus::Utility::Logger::_logger->error(__VA_ARGS__);

#else

#define NEXUS_LOG_INIT
#define NEXUS_LOG_SHUT

#define NEXUS_LOG_DEBUG(...)
#define NEXUS_LOG_TRACE(...)
#define NEXUS_LOG_INFO(...)
#define NEXUS_LOG_WARN(...)
#define NEXUS_LOG_ERROR(...)

#endif