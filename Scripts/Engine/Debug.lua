project "Debug"
    kind "SharedLib"
    language "C++"
    location (EngDir.."Debug")

    defines "NEXUS_DEBUG_DLL"

    targetdir(BinDir)
    objdir(IntDir)

    includedirs
    {
        IncludeDir["Debug"],
    }

    files
    {
        (EngDir.."Debug/**.h"),
        (EngDir.."Debug/**.cpp")
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
