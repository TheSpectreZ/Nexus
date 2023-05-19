#include "nxpch.h"
#include "Timer.h"

std::unordered_map<const char*, float> Nexus::Timer::Profiles;

Nexus::Timer::Timer(const char* name)
	:m_Name(name),m_Stopped(false)
{
	m_StartPoint = std::chrono::high_resolution_clock::now();
}

Nexus::Timer::~Timer()
{
	if (!m_Stopped)
		Stop();
}

void Nexus::Timer::Stop()
{
	auto endPoint = std::chrono::high_resolution_clock::now();

	auto start = std::chrono::time_point_cast<std::chrono::milliseconds>(m_StartPoint).time_since_epoch().count();
	auto end = std::chrono::time_point_cast<std::chrono::milliseconds>(endPoint).time_since_epoch().count();
	
	m_Stopped = true;

	float dt = (float)(end - start) ;

	Profiles[m_Name] = dt;
}
