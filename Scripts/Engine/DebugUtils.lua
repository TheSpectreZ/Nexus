project "DebugUtils"
    kind "SharedLib"
    language "C++"
    location (EngDir.."DebugUtils")

    defines "NEXUS_DEBUG_DLL"

    targetdir(BinDir)
    objdir(IntDir)

    includedirs
    {
        IncludeDir["DebugUtils"],

        IncludeDir["spdlog"],
    }

    files
    {
        (EngDir.."DebugUtils/**.h"),
        (EngDir.."DebugUtils/**.cpp")
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
