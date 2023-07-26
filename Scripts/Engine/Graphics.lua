project "NxGraphics"
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
		"$(VULKAN_SDK)/Include",
		IncludePath["utils"],
		IncludePath["NxCore"],
		IncludePath["NxGraphics"],
	}

	links
	{
		"NxCore",
	}

	libdirs
	{
		"$(VULKAN_SDK)/Lib"
	}

	linkoptions 
	{
		"-IGNORE:4098",
		"-IGNORE:4099", 
		"-IGNORE:4006", 
	}

	defines "NEXUS_GRAPHICS_SHARED_BUILD"

	filter "system:windows"
		systemversion "latest"
		defines "NEXUS_SYSTEM_WINDOWS"
		disablewarnings { "4251","4275" }

	filter "configurations:Debug"
		links { "shaderc_combinedd.lib" }
		optimize "Off"
		symbols "Full"
		defines "NEXUS_DEBUG"

	filter "configurations:Release"
		links { "shaderc_combined.lib" }
		optimize "Speed"
		symbols "FastLink"
		defines "NEXUS_RELEASE"

	filter "configurations:Dist"
		links { "shaderc_combined.lib" }
		optimize "Full"
		symbols "Off"
		defines "NEXUS_DIST"