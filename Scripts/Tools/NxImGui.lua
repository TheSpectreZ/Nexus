project "NxImGui"
	kind "SharedLib"
	language "C++"
	cppdialect "C++20"
	location (SrcDir.."Tools/%{prj.name}")

	targetdir (BinDir)
	objdir (IntDir)

	files
	{
		(SrcDir.."Tools/%{prj.name}/include/%{prj.name}/**.h"),
		(SrcDir.."Tools/%{prj.name}/src/**.cpp")
	}

	includedirs
	{
		"$(VULKAN_SDK)/Include",
		IncludePath["utils"],
		IncludePath["imgui"],

		IncludePath["NxCore"],
		IncludePath["NxVulkan"],
		IncludePath["NxGraphics"],
		IncludePath["NxRenderEngine"],
		
		IncludePath["NxImGui"],
	}

	links
	{
		"vulkan-1.lib",
		"imgui",

		"NxCore",
		"NxVulkan",
		"NxRenderEngine",
	}

	libdirs
	{
		"$(VULKAN_SDK)/Lib"
	}

	defines
	{
		"NEXUS_IMGUI_SHARED_BUILD"
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