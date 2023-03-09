#include "nxpch.h"

#include "spdlog/sinks/stdout_color_sinks.h"

std::shared_ptr<spdlog::logger> Nexus::Utility::Logger::_logger;

void Nexus::Utility::Logger::Init()
{
	spdlog::set_pattern("NEXUS::%T :%^ %v %$");

	_logger = spdlog::stdout_color_mt("NEXUS");
	_logger->set_level(spdlog::level::trace);
}

void Nexus::Utility::Logger::Shut()
{
	spdlog::shutdown();
}
