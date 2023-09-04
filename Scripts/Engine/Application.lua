project "NxApplication"
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
		IncludePath["glfw"],
		
		IncludePath["NxCore"],
		IncludePath["NxAsset"],
		IncludePath["NxScene"],
		IncludePath["NxGraphics"],
		IncludePath["NxRenderEngine"],
		IncludePath["NxPhysicsEngine"],
		IncludePath["NxScriptEngine"],
		IncludePath["NxNetworkEngine"],
		IncludePath["NxApplication"]
	}

	links
	{
		"glfw",
		"NxCore",
		"NxScene",
		"NxAsset",
		"NxRenderEngine",
		"NxPhysicsEngine",
		"NxScriptEngine",
		"NxNetworkEngine"
	}

	defines "NEXUS_ENGINE_SHARED_BUILD"

	filter "system:windows"
		systemversion "latest"
		defines "NEXUS_SYSTEM_WINDOWS"
		disablewarnings { "4251","4275" }

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