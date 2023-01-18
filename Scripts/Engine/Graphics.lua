project "Graphics"
	kind "SharedLib"
	language "C++"
	location (EngDir.."Graphics")

	defines "NEXUS_GRAPHICS_DLL"

	targetdir(BinDir)
	objdir(IntDir)

	includedirs
	{
		IncludeDir["Graphics"],
	
		IncludeDir["DebugUtils"],

		IncludeDir["glfw"],
		IncludeDir["spdlog"],
		IncludeDir["vulkanSDK"],
	}

	files
	{
		(EngDir.."Graphics/**.h"),
		(EngDir.."Graphics/**.cpp"),
	}

	links 
	{
		"DebugUtils",

		"glfw",
		"vulkan-1.lib",
	}

	libdirs
	{
		LibDir["vulkanSDK"]
	}

	filter "system:windows"
		defines "NEXUS_SYSTEM_WINDOWS"
		systemversion "latest"
		cppdialect "C++20"

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
