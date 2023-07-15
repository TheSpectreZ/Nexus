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
	enum class NEXUS_CORE_API LoggerType
	{
		Console, File
	};

	class NEXUS_CORE_API Logger
	{
	public:
		virtual void Log(const char* Entry,int line, const char* file, const char* format, ...) = 0;
		virtual ~Logger() = default;
	};

	class NEXUS_CORE_API ConsoleLogger : public Logger
	{
	public:
		ConsoleLogger();
		~ConsoleLogger() override;

		void Log(const char* Entry, int line, const char* file, const char* format, ...) override;
	};

	class NEXUS_CORE_API FileLogger : public Logger
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
		static LogManager* s_Instance;
	public:
		static LogManager* Get() { return s_Instance; }
		static void Initialize();
		static void Shutdown();

		template<typename... Args>
		void Make(LoggerType Type, Args... args)
		{
			switch (Type)
			{
			case Nexus::LoggerType::Console:
				m_Loggers.push_back(CreateRef<ConsoleLogger>()); break;
			case Nexus::LoggerType::File:
				m_Loggers.push_back(CreateRef<FileLogger>(args...)); break;
			default:
				return;
			}
		}

		template<typename... Args>
		void Log(const char* Entry, int line, const char* file, const char* format, Args... args)
		{
			for (auto& logger : m_Loggers)
				logger->Log(Entry, line, file, format, args...);
		}
	private:
		std::vector< Ref<Logger> > m_Loggers;
	};

#define NEXUS_LOG(Entry,log,...) ::Nexus::LogManager::Get()->Log(Entry,__LINE__,__FILE__,log,##__VA_ARGS__)

}
