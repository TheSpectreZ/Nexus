project "NxScriptEngine"
	kind "SharedLib"
	language "C++"
	cppdialect "C++20"
	location (SrcDir.."Engine/%{prj.name}")

	targetdir (BinDir)
	objdir (IntDir)

	files
	{
		(SrcDir.."Engine/%{prj.name}/include/**.h"),
		(SrcDir.."Engine/%{prj.name}/src/**.cpp")
	}

	includedirs
	{
		IncludePath["utils"],
		IncludePath["mono"],
		
		IncludePath["NxCore"],
		IncludePath["NxScene"],
		IncludePath["NxScriptEngine"],
	}

	links
	{
		"NxCore",
		"NxScene",
		"libmono-static-sgen.lib"
	}

	defines "NEXUS_SCRIPT_ENGINE_SHARED_BUILD"

	filter "system:windows"
		systemversion "latest"
		defines "NEXUS_SYSTEM_WINDOWS"
		disablewarnings { "4251" }
		links
        {
            "Ws2_32.lib",
            "Winmm.lib",
            "Version.lib",
            "Bcrypt.lib",
        }

	filter "configurations:Debug"
		optimize "Off"
		symbols "Full"
		defines "NEXUS_DEBUG"
		libdirs (VenDir.."Mono/lib/Debug")
	
	filter "configurations:Release"
		optimize "Speed"
		symbols "FastLink"
		defines "NEXUS_RELEASE"
		libdirs (VenDir.."Mono/lib/Release")

	filter "configurations:Dist"
		optimize "Full"
		symbols "Off"
		defines "NEXUS_DIST"
		libdirs (VenDir.."Mono/lib/Release")