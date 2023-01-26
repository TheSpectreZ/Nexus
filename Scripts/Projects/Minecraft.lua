project "Minecraft"
    kind "ConsoleApp"
    language "C++"
    location (AppDir.."Minecraft")

    targetdir(BinDir)
    objdir(IntDir)

    includedirs
    {
        IncludeDir["Core"],
        IncludeDir["Graphics"],
        IncludeDir["DebugUtils"],
        IncludeDir["Platform"],

        IncludeDir["spdlog"],
        IncludeDir["vulkanSDK"],
    }

    files
    {
        (AppDir.."Minecraft/**.h"),
        (AppDir.."Minecraft/**.cpp")
    }

    links
    {
        "Core",
        "Graphics",
        "DebugUtils",
        "Platform"
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
