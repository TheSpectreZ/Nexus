project "NxTestClient"
	kind "WindowedApp"
	language "C++"
	cppdialect "C++20"
	location (SrcDir.."Tools/%{prj.name}")

	targetdir (BinDir)
	objdir (IntDir)

	files
	{
		(SrcDir.."Tools/%{prj.name}/src/**.h"),
		(SrcDir.."Tools/%{prj.name}/src/**.cpp")
	}

	links
	{
		"NxCore",
		"NxNetworkEngine",
	}

	includedirs
	{
		IncludePath["utils"],
		
		IncludePath["NxCore"],
		IncludePath["NxNetworkEngine"],
	}

	libdirs
	{
	}

	filter "system:windows"
		systemversion "latest"
		defines "NEXUS_SYSTEM_WINDOWS"
		disablewarnings { "4251","4996","4275" }

	filter "configurations:Debug"
		optimize "Off"
		symbols "Full"
		defines "NEXUS_DEBUG"

	filter "configurations:Release"
		optimize "Speed"
		symbols "FastLink"
		defines "NEXUS_RELEASE"

	filter "configurations:Dist"
		optimize "Full"
		symbols "Off"
		defines "NEXUS_DIST"