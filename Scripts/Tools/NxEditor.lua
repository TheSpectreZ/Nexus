project "NxEditor"
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
		"vulkan-1.lib",
		"imgui",
		"NxCore",
		"NxScene",
		"NxAsset",
		"NxGraphics",
		"NxVulkan",
		"NxRenderEngine",
		"NxPhysicsEngine",
		"NxApplication",
		"NxScriptEngine",

		"NxImGui"
	}

	includedirs
	{
		"$(VULKAN_SDK)/Include",
		IncludePath["utils"],
		IncludePath["imgui"],
		
		IncludePath["NxCore"],
		IncludePath["NxScene"],
		IncludePath["NxAsset"],
		IncludePath["NxGraphics"],
		IncludePath["NxVulkan"],
		IncludePath["NxRenderEngine"],
		IncludePath["NxPhysicsEngine"],
		IncludePath["NxScriptEngine"],
		IncludePath["NxApplication"],

		IncludePath["NxImGui"]
	}

	libdirs
	{
		"$(VULKAN_SDK)/Lib"
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