project "Editor"
    language "C++"
    location (PrmDir.."Editor")

    targetdir(BinDir)
    objdir(IntDir)

    includedirs
    {
        IncludeDir["Platform"],
        IncludeDir["Graphics"],
        IncludeDir["Utility"],
        IncludeDir["Config"],

        IncludeDir["spdlog"],
        IncludeDir["imgui"],
        IncludeDir["vulkanSDK"],
    }

    files
    {
        (PrmDir.."Editor/**.h"),
        (PrmDir.."Editor/**.cpp")
    }

    links
    {
        "Platform",
        "Graphics",
        "Utility",
        "Config",
        
        "imgui",
        "spdlog",
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
