#pragma once
#include "Base.h"
#include <fstream>


#ifdef NEXUS_CORE_SHARED_BUILD
#define NEXUS_CORE_API __declspec(dllexport)
#else 
#define NEXUS_CORE_API __declspec(dllimport)
#endif // NEXUS_CORE_SHARED_BUILD

namespace Nexus
{
	class NEXUS_CORE_API ILogger
	{
	public:
		virtual void Log(const char* Entry,int line, const char* file, const char* format, ...) = 0;
		virtual ~ILogger() = default;
	};

	class NEXUS_CORE_API ConsoleLogger : public ILogger
	{
	public:
		ConsoleLogger();
		~ConsoleLogger() override;

		void Log(const char* Entry, int line, const char* file, const char* format, ...) override;
	};

	class NEXUS_CORE_API FileLogger : public ILogger
	{
	public:
		FileLogger();
		~FileLogger() override;

		void Log(const char* Entry, int line, const char* file, const char* format, ...) override;
	private:
		std::ofstream _stream;
	};

	class NEXUS_CORE_API LogManager
	{
	public:
		static LogManager& Instance();
		
		template<typename T> requires std::derived_from<T, ILogger>
		void Make()
		{
			m_Loggers.push_back(std::make_shared<T>());
		}

		template<typename... Args>
		void Log(const char* Entry, int line, const char* file, const char* format, Args... args)
		{
			for (const auto& logger : m_Loggers)
				logger->Log(Entry, line, file, format, args...);
		}
	private:
		std::vector< std::shared_ptr<ILogger> > m_Loggers;
	};

#define NEXUS_LOG(Entry,log,...) ::Nexus::LogManager::Instance().Log(Entry,__LINE__,__FILE__,log,##__VA_ARGS__)

}
