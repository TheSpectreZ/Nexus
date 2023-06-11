#include "NxCore/Logger.h"
#include <Windows.h>
#include <filesystem>
#include <iostream>

Nexus::ConsoleLogger::ConsoleLogger()
{
	AllocConsole();

	FILE* file;
	freopen_s(&file,"conin$", "r", stdin);
	freopen_s(&file,"conout$", "w", stdout);
	freopen_s(&file,"conout$", "w", stderr);
}

Nexus::ConsoleLogger::~ConsoleLogger()
{
	std::cout << "\nPress any key to Continue...\n";
	std::cin.get();
	FreeConsole();
}

void Nexus::ConsoleLogger::Log(const char* Entry, int line, const char* file, const char* format, ...)
{
	std::filesystem::path path = file;

	char finalBuffer[1024 + 100];
	snprintf(finalBuffer, 100, "[%s] | %s : %d | ", Entry, path.filename().string().c_str(), line);

	char buffer[1024];
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, 1024, format, args);
	va_end(args);

	strncat_s(finalBuffer, buffer, 1024);


	printf("%s\n", finalBuffer);
}

Nexus::FileLogger::FileLogger()
{
	std::string buffer(128, '\0');
	auto timeTApp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	
	tm appLT;
	localtime_s(&appLT, &timeTApp);

	strftime(&buffer[0], buffer.size(), " %Y-%m-%d@%H-%M LOG.txt", &appLT);

	std::filesystem::path filePath = std::filesystem::current_path() / "Logs";
	std::filesystem::create_directories(filePath);
	filePath /= buffer;

	_stream = std::ofstream(filePath.string().c_str(), std::ifstream::binary | std::ifstream::out);
}

Nexus::FileLogger::~FileLogger()
{
	_stream.close();
}

void Nexus::FileLogger::Log(const char* Entry, int line, const char* file, const char* format, ...)
{
	std::filesystem::path path = file;

	char finalBuffer[1024 + 100];
	snprintf(finalBuffer, 100, "[%s] | %s : %d | ", Entry, path.filename().string().c_str(), line);

	char buffer[1024];
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, 1024, format, args);
	va_end(args);

	strncat_s(finalBuffer, buffer, 1024);

	_stream << finalBuffer;
	_stream << std::endl;
}

Nexus::LogManager* Nexus::LogManager::s_Instance = nullptr;

void Nexus::LogManager::Initialize()
{
	s_Instance = new LogManager;
}

void Nexus::LogManager::Shutdown()
{
	s_Instance->m_Loggers.clear();
	delete s_Instance;
}