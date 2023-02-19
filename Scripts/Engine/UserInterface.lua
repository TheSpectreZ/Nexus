project "UserInterface"
    kind "SharedLib"
    language "C++"
    location (EngDir.."UserInterface")

    defines "NEXUS_USER_INTERFACE_DLL"

    targetdir(BinDir)
    objdir(IntDir)

    includedirs
    {
        IncludeDir["UserInterface"],

        IncludeDir["Platform"],
        IncludeDir["Graphics"],
        IncludeDir["Utility"],

        IncludeDir["glfw"],
        IncludeDir["spdlog"],
        IncludeDir["imgui"],
        IncludeDir["vulkanSDK"],
    }

    files
    {
        (EngDir.."UserInterface/**.h"),
        (EngDir.."UserInterface/**.cpp")
    }

    links
    {
        "Platform",
        "Graphics",
        "Utility",

        "vulkan-1.lib",
        "glfw",
        "imgui"
    }

    libdirs
    {
        LibDir["vulkanSDK"]
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
