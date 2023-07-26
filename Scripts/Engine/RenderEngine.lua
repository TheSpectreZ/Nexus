project "NxRenderEngine"
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
		IncludePath["NxVulkan"],
		IncludePath["NxGraphics"],
		IncludePath["NxScene"],
		IncludePath["NxAsset"],

		IncludePath["NxRenderEngine"],
	}

	links
	{
		"NxCore",
		"NxGraphics",
		"NxVulkan",
		"NxScene",
		"NxAsset"
	}

	defines 
	{
		"NEXUS_RENDERER_SHARED_BUILD"
	}

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