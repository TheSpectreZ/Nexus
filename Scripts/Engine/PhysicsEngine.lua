project "NxPhysicsEngine"
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
		IncludePath["physx"],

		IncludePath["NxCore"],
		IncludePath["NxScene"],

		IncludePath["NxGraphics"],
		IncludePath["NxRenderEngine"],

		IncludePath["NxPhysicsEngine"],
	}

	links
	{
		"NxCore",
		"NxScene",
		"NxRenderEngine",

		"PhysX_64.lib",
		"PhysXCommon_64.lib",
		"PhysXFoundation_64.lib",
		"PhysXCooking_64.lib",
		"PhysXExtensions_static_64.lib",
		"PhysXPvdSDK_static_64.lib",
	}

	libdirs
	{
		BinDir
	}

	defines 
	{
		"NEXUS_PHYSICS_ENGINE_SHARED_BUILD",
		"_SILENCE_ALL_CXX20_DEPRECATION_WARNINGS"
	}

	linkoptions 
	{
		"-IGNORE:4098",
		"-IGNORE:4099", 
		"-IGNORE:4006", 
	}

	filter "system:windows"
		systemversion "latest"
		staticruntime "On"
		defines 
		{
			"NEXUS_SYSTEM_WINDOWS"
		}
		disablewarnings { "4251","4275" }

	filter "configurations:Debug"
		optimize "Off"
		symbols "Full"
		defines "NEXUS_DEBUG"
		
	filter "configurations:Release"
		optimize "Speed"
		symbols "FastLink"
		defines
		{
			"NEXUS_RELEASE","NDEBUG"
		}
		
	filter "configurations:Dist"
		optimize "Full"
		symbols "Off"
		defines
		{
			"NEXUS_DIST","NDEBUG"
		}