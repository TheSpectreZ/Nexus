project "NexusEditor"
    language "C++"
    cppdialect "C++20"
    location (SrcDir.."%{prj.name}")

    targetdir(BinDir)
    objdir(IntDir)

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
        IncludeDir["vulkanMemoryAllocator"],
        "$(VULKAN_SDK)/Include"
    }

    links
    {
        "Nexus",

        "vulkan-1.lib"
    }

    libdirs
    {
        "$(VULKAN_SDK)/Lib"
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