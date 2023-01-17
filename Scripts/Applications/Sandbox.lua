project "Sandbox"
    kind "ConsoleApp"
    language "C++"
    location (AppDir.."Sandbox")

    targetdir(BinDir)
    objdir(IntDir)

    includedirs
    {
        (AppDir.."Sandbox/include")
    }

    files
    {
        (AppDir.."Sandbox/**.h"),
        (AppDir.."Sandbox/**.cpp")
    }

    links
    {

    }

    libdirs
    {

    }

    filter "system:windows"
        defines "NEXUS_PLATFORM_WINDOW"
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
