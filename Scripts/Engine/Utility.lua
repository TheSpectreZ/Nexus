project "Utility"
    kind "SharedLib"
    language "C++"
    location (EngDir.."Utility")

    defines "NEXUS_UTILITY_DLL"

    targetdir(BinDir)
    objdir(IntDir)

    includedirs
    {
        IncludeDir["Utility"],

        IncludeDir["spdlog"],
        IncludeDir["stb"],
        IncludeDir["entt"],
    }

    files
    {
        (EngDir.."Utility/**.h"),
        (EngDir.."Utility/**.cpp")
    }

    links
    {
        "spdlog"
    }

    filter "system:windows"
        defines "NEXUS_SYSTEM_WINDOWS"
        cppdialect "C++20"
        systemversion "latest"
        disablewarnings { "4251" }

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
