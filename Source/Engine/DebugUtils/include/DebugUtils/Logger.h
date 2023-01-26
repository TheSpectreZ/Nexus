#pragma once
#include "Build.h"
#include "spdlog/spdlog.h"
#include <memory>

namespace Nexus
{
	namespace DebugUtils
	{
		struct NEXUS_DEBUG_API Logger
		{
			static void Init();
			static void Shut();

			static std::shared_ptr<spdlog::logger> _logger;
		};
	}
}

#ifdef NEXUS_DEBUG

#define NEXUS_LOG_INIT ::Nexus::DebugUtils::Logger::Init();
#define NEXUS_LOG_SHUT ::Nexus::DebugUtils::Logger::Shut();

#define NEXUS_LOG_DEBUG(...) ::Nexus::DebugUtils::Logger::_logger->debug(__VA_ARGS__);
#define NEXUS_LOG_TRACE(...) ::Nexus::DebugUtils::Logger::_logger->trace(__VA_ARGS__);
#define NEXUS_LOG_INFO(...) ::Nexus::DebugUtils::Logger::_logger->info(__VA_ARGS__);
#define NEXUS_LOG_WARN(...) ::Nexus::DebugUtils::Logger::_logger->warn(__VA_ARGS__);
#define NEXUS_LOG_ERROR(...) ::Nexus::DebugUtils::Logger::_logger->error(__VA_ARGS__);

#else

#define NEXUS_LOG_INIT
#define NEXUS_LOG_SHUT


#define NEXUS_LOG_DEBUG(...)
#define NEXUS_LOG_TRACE(...)
#define NEXUS_LOG_INFO(...)
#define NEXUS_LOG_WARN(...)
#define NEXUS_LOG_ERROR(...)

#endif