#pragma once

namespace Nexus
{
	class Timer
	{
	public:
		static std::unordered_map<const char*, float> Profiles;
		Timer(const char* name);
		~Timer();

		void Stop();
	private:
		const char* m_Name;
		std::chrono::time_point<std::chrono::steady_clock> m_StartPoint;
		bool m_Stopped;
	};
}

#define NEXUS_SCOPED_PROFILE(name) Nexus::Timer timer__LINE__(name)