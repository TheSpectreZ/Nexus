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
        IncludeDir["assimp"],
        IncludeDir["imgui"],
        IncludeDir["vulkanMemoryAllocator"],
        IncludeDir["mono"],
        IncludeDir["joltRoot"],
        IncludeDir["jolt"],
        IncludeDir["tinygltf"],
        IncludeDir["filewatch"],

        "$(VULKAN_SDK)/Include",
    }

    links
    {
        "glfw",
        "spdlog",
        "yamlcpp",
        "assimp",
        "imgui",
        "jolt",
        "vulkan-1.lib",
        "libmono-static-sgen.lib"
    }

    libdirs
    {
        LibDir["vulkanSDK"]
    }

    disablewarnings
    {
        "4005",
        "4275",
        "4251"
    }
    
    linkoptions 
	{
		"-IGNORE:4098",
		"-IGNORE:4099", 
		"-IGNORE:4006", 
	}

    filter "system:windows"
		systemversion "latest"
        links
        {
            "Ws2_32.lib",
            "Winmm.lib",
            "Version.lib",
            "Bcrypt.lib",
        }

	filter "configurations:Debug"
        defines 
        {
            "NEXUS_DEBUG",
            "JPH_PROFILE_ENABLED"
        }
        optimize "Off"
        symbols "Full"
        links
        {
            "shaderc_combinedd.lib",
            "spirv-cross-glsld.lib",
            "spirv-cross-cored.lib",
        }
        libdirs
        {
            (VenDir.."Mono/lib/debug"),
        }

    filter "configurations:Release"
        defines 
        {
            "NEXUS_RELEASE",
            "JPH_PROFILE_ENABLED"
        }
        optimize "Speed"
        symbols "FastLink"
        links
        {
            "shaderc_combined.lib",
            "spirv-cross-glsl.lib",
            "spirv-cross-core.lib",
        }
        libdirs
        {
            (VenDir.."Mono/lib/release"),
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
        libdirs
        {
            (VenDir.."Mono/lib/release"),
        }