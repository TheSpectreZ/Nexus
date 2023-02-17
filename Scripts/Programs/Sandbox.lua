project "Sandbox"
    language "C++"
    location (PrmDir.."Sandbox")

    targetdir(BinDir)
    objdir(IntDir)

    includedirs
    {
        IncludeDir["Application"],
        IncludeDir["Platform"],
        IncludeDir["UserInterface"],
        IncludeDir["Graphics"],
    }

    files
    {
        (PrmDir.."Sandbox/**.h"),
        (PrmDir.."Sandbox/**.cpp")
    }

    links
    {
        "Application","Platform","UserInterface","Graphics"
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
