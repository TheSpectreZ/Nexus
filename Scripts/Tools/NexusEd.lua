project "NexusEd"
	kind "WindowedApp"
	language "C++"
	cppdialect "C++20"
	location (SrcDir.."Tools/%{prj.name}")

	targetdir (BinDir)
	objdir (IntDir)

	characterset "MBCS"

	files
	{
		(SrcDir.."Tools/%{prj.name}/include/**.h"),
		(SrcDir.."Tools/%{prj.name}/src/**.cpp")
	}

	links
	{
		"Engine"
	}

	includedirs
	{
		IncludePath["Core"],
		IncludePath["Engine"]
	}

	filter "system:windows"
		systemversion "latest"
		defines "NEXUS_SYSTEM_WINDOWS"

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