project "NxDirect3D"
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

		IncludePath["NxDirect3D"]
	}

	links
	{
		"NxCore",
		"NxGraphics",
	}

	libdirs
	{
		"$(VULKAN_SDK)/Lib"
	}

	defines 
	{
		"NEXUS_DIRECTX_SHARED_BUILD"
	}

	linkoptions 
	{
		"-IGNORE:4098",
		"-IGNORE:4099", 
		"-IGNORE:4006", 
	}

	filter "system:windows"
		systemversion "latest"
		defines 
		{
			"NEXUS_SYSTEM_WINDOWS"
		}
		disablewarnings { "4251","4275" }

	filter "configurations:Debug"
		optimize "Off"
		symbols "Full"
		defines "NEXUS_DEBUG"
		links
        {
            "spirv-cross-hlsld.lib",
            "spirv-cross-cored.lib",
        }

	filter "configurations:Release"
		optimize "Speed"
		symbols "FastLink"
		defines "NEXUS_RELEASE"
		links
        {
            "spirv-cross-hlsl.lib",
            "spirv-cross-core.lib",
        }

	filter "configurations:Dist"
		optimize "Full"
		symbols "Off"
		defines "NEXUS_DIST"
		links
        {
            "spirv-cross-hlsl.lib",
            "spirv-cross-core.lib",
        }