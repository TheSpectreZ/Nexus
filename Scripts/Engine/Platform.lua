project "Platform"
    kind "SharedLib"
    language "C++"
    location (EngDir.."Platform")

    defines "NEXUS_PLATFORM_DLL"

    targetdir(BinDir)
    objdir(IntDir)

    includedirs
    {
        IncludeDir["Platform"],

        IncludeDir["DebugUtils"],

        IncludeDir["glfw"],
        IncludeDir["spdlog"],
    }

    files
    {
        (EngDir.."Platform/**.h"),
        (EngDir.."Platform/**.cpp")
    }

    links
    {
        "DebugUtils",

        "glfw"
    }

    filter "system:windows"
        defines "NEXUS_SYSTEM_WINDOWS"
        cppdialect "C++20"
        systemversion "latest"

    filter "configurations:Debug"
        defines "NEXUS_DEBUG"
        optimize "Off"
        symbols "Full"

    filter "configurations:Release"
        defines "NEXUS_RELEASE"
        optimize "Speed"
        symbols "FastLink"

    filter "configurations:Dist"
        defines "NEXUS_DIST"
        optimize "Full"
        symbols "Off"
