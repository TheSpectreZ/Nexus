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
		IncludeDir["Platform"],
		IncludeDir["DebugUtils"],

		IncludeDir["glfw"],
		IncludeDir["spdlog"],
		IncludeDir["vulkanSDK"],
		IncludeDir['vulkanMemoryAllocator']
	}

	files
	{
		(EngDir.."Graphics/**.h"),
		(EngDir.."Graphics/**.cpp"),
	}

	links 
	{
		"DebugUtils",
		"Platform",

		"glfw",
		"vulkan-1.lib",
	}

	libdirs
	{
		LibDir["vulkanSDK"]
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
		links "shaderc_combinedd.lib"
        defines "NEXUS_DEBUG"
        optimize "Off"
        symbols "Full"

    filter "configurations:Release"
		links "shaderc_combined.lib"
        defines "NEXUS_RELEASE"
        optimize "Speed"
        symbols "FastLink"

    filter "configurations:Dist"
		links "shaderc_combined.lib"
        defines "NEXUS_DIST"
        optimize "Full"
        symbols "Off"
