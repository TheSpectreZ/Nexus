project "NxTestServer"
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
		"imgui",

		"NxCore",
		"NxScene",
		"NxGraphics",
		"NxRenderEngine",
		"NxNetworkEngine",
		"NxApplication",
		
		"NxImGui"
	}

	includedirs
	{
		IncludePath["utils"],
		
		IncludePath["imgui"],
		
		IncludePath["NxCore"],
		IncludePath["NxScene"],
		IncludePath["NxGraphics"],
		IncludePath["NxRenderEngine"],
		IncludePath["NxNetworkEngine"],
		IncludePath["NxApplication"],

		IncludePath["NxImGui"]
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