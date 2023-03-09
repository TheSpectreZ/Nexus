project "Nexus"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    location (SrcDir.."%{prj.name}")

    targetdir(BinDir)
    objdir(IntDir)

    characterset "MBCS"

    pchheader "nxpch.h"
	pchsource (SrcDir.."%{prj.name}/src/nxpch.cpp")

    files
    {
        (SrcDir.."%{prj.name}/**.h"),
        (SrcDir.."%{prj.name}/**.cpp"),
    }

    includedirs
    { 
        (SrcDir.."Nexus/src/"),
        IncludeDir["Nexus"],

        IncludeDir["glfw"],
        IncludeDir["spdlog"],
        IncludeDir["stb"],
        IncludeDir["entt"],
        IncludeDir["yamlcpp"],
        IncludeDir["vulkanMemoryAllocator"],

        "$(VULKAN_SDK)/Include",
    }

    links
    {
        "glfw",
        "spdlog",
        "yamlcpp",
        "vulkan-1.lib",
    }

    libdirs
    {
        LibDir["vulkanSDK"]
    }

    disablewarnings
    {
        "4005"
    }
    
    linkoptions 
	{
		"-IGNORE:4098",
		"-IGNORE:4099", 
	}

    filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
        defines "NEXUS_DEBUG"
        optimize "Off"
        symbols "Full"
        links
        {
            "shaderc_combinedd.lib",
            "spirv-cross-glsld.lib",
            "spirv-cross-cored.lib",
        }

    filter "configurations:Release"
        defines "NEXUS_RELEASE"
        optimize "Speed"
        symbols "FastLink"
        links
        {
            "shaderc_combined.lib",
            "spirv-cross-glsl.lib",
            "spirv-cross-core.lib",
        }

    filter "configurations:Dist"
        defines "NEXUS_DIST"
        optimize "Full"
        symbols "Off"
        links
        {
            "shaderc_combined.lib",
            "spirv-cross-glsl.lib",
            "spirv-cross-core.lib",
        }