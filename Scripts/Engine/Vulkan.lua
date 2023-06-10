project "NxVulkan"
	kind "SharedLib"
	language "C++"
	cppdialect "C++20"
	location (SrcDir.."Engine/%{prj.name}")

	targetdir (BinDir)
	objdir (IntDir)

	characterset "MBCS"

	files
	{
		(SrcDir.."Engine/%{prj.name}/include/**.h"),
		(SrcDir.."Engine/%{prj.name}/src/**.cpp")
	}

	includedirs
	{
		"$(VULKAN_SDK)/Include",
		IncludePath["VMA"],
		IncludePath["NxCore"],
		IncludePath["NxVulkan"],
		IncludePath["NxGraphics"],
	}

	links
	{
		"NxCore",
		"vulkan-1.lib"
	}

	libdirs
	{
		"$(VULKAN_SDK)/Lib"
	}

	defines 
	{
		"NEXUS_VULKAN_SHARED_BUILD"
	}

	filter "system:windows"
		systemversion "latest"
		defines 
		{
			"VK_USE_PLATFORM_WIN32_KHR",
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
		defines "NEXUS_RELEASE"

	filter "configurations:Dist"
		optimize "Full"
		symbols "Off"
		defines "NEXUS_DIST"