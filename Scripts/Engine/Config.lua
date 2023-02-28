project "Config"
    language "C++"
    kind "SharedLib"
    location (EngDir.."Config")

    targetdir(BinDir)
    objdir(IntDir)

    includedirs
    {
        IncludeDir["Config"],

        IncludeDir["yamlcpp"],
    }

    files
    {
        (EngDir.."Config/**.h"),
        (EngDir.."Config/**.cpp")
    }

    links
    {
        "yamlcpp",
    }

    defines "NEXUS_SERIALIZER_DLL"

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
