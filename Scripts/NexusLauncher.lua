project "NexusLauncher"
    language "C++"
    cppdialect "C++20"
    location (SrcDir.."%{prj.name}")

    targetdir (BinDir)
    objdir (IntDir)
    debugdir (SrcDir.."NexusEditor")

    files
    {
        (SrcDir.."%{prj.name}/**.h"),
        (SrcDir.."%{prj.name}/**.cpp"),
    }

    includedirs
    {
        IncludeDir["Nexus"],

        IncludeDir["spdlog"],
        IncludeDir["entt"],
        IncludeDir["imgui"],

        "$(VULKAN_SDK)/Include"
    }

    links
    {
        "Nexus",
    }
    
    linkoptions 
	{
		"-IGNORE:4098",
		"-IGNORE:4099", 
	}

    filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
        kind "ConsoleApp"
        defines "NEXUS_DEBUG"
        optimize "Off"
        symbols "Full"

    filter "configurations:Release"
        kind "ConsoleApp"
        defines "NEXUS_RELEASE"
        optimize "Speed"
        symbols "FastLink"

    filter "configurations:Dist"
        kind "WindowedApp"
        defines "NEXUS_DIST"
        optimize "Full"
        symbols "Off"