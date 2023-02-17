project "Application"
    kind "SharedLib"
    language "C++"
    location (EngDir.."Application")

    defines "NEXUS_APPLICATION_DLL"

    targetdir(BinDir)
    objdir(IntDir)

    includedirs
    {
        IncludeDir["Application"],

        IncludeDir["DebugUtils"],
        IncludeDir["Graphics"],
        IncludeDir["Audio"],
        IncludeDir["Platform"],
        IncludeDir["UserInterface"],

        IncludeDir["spdlog"],
        IncludeDir["vulkanSDK"],
    }

    files
    {
        (EngDir.."Application/**.h"),
        (EngDir.."Application/**.cpp")
    }

    links
    {
        "DebugUtils",
        "Platform",
        "Graphics",
        "UserInterface",
        "Audio",
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
