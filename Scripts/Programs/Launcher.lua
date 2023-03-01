project "Launcher"
    language "C++"
    location (PrmDir.."Launcher")

    targetdir(BinDir)
    objdir(IntDir)

    includedirs
    {
        IncludeDir["Platform"],
        IncludeDir["Utility"],
        IncludeDir["Config"],

        IncludeDir["spdlog"],
        IncludeDir["imgui"],
        IncludeDir["glfw"],
        IncludeDir["glad"],
    }

    files
    {
        (PrmDir.."Launcher/**.h"),
        (PrmDir.."Launcher/**.cpp")
    }

    links
    {
        "Platform",
        "Utility",
        "Config",

        "imgui",
        "glfw",
        "glad"
    }

    filter "system:windows"
        defines "NEXUS_SYSTEM_WINDOWS"
        cppdialect "C++20"
        systemversion "latest"
        disablewarnings { "4251" }

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