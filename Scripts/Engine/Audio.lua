project "Audio"
	kind "SharedLib"
	language "C++"
	location (EngDir.."Audio")

	defines "NEXUS_AUDIO_DLL"

	targetdir(BinDir)
	objdir(IntDir)

	includedirs
	{
		IncludeDir["Audio"],
		IncludeDir["Utility"],

		IncludeDir["openALSoft"],
		IncludeDir["spdlog"]
	}

	files
	{
		(EngDir.."Audio/**.h"),
		(EngDir.."Audio/**.cpp"),
	}

	links 
	{
		"Utility",

		"openALSoft",
	}

	defines
	{
		"AL_LIBTYPE_STATIC"
	}

	linkoptions 
	{
		"-IGNORE:4098",
		"-IGNORE:4099", 
	}

	filter "system:windows"
		defines "NEXUS_SYSTEM_WINDOWS"
		systemversion "latest"
		cppdialect "C++20"
		disablewarnings { "4251" }

	filter "configurations:Debug"
		defines "NEXUS_DEBUG"
        optimize "Off"
        symbols "Full"

    filter "configurations:Release"
		defines "NEXUS_RELEASE"
        optimize "Speed"
        symbols "FastLink"

    filter "configurations:Dist"
		defines "NEXUS_DIST"
        optimize "Full"
        symbols "Off"
