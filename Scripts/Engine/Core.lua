project "Core"
    kind "SharedLib"
    language "C++"
    location (EngDir.."Core")

    defines "NEXUS_CORE_DLL"

    targetdir(BinDir)
    objdir(IntDir)

    includedirs
    {
        IncludeDir["Core"],

        IncludeDir["DebugUtils"],
        IncludeDir["Graphics"],
        IncludeDir["Platform"],

        IncludeDir["spdlog"],
        IncludeDir["vulkanSDK"],
    }

    files
    {
        (EngDir.."Core/**.h"),
        (EngDir.."Core/**.cpp")
    }

    links
    {
        "DebugUtils",
        "Platform",
        "Graphics"
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
