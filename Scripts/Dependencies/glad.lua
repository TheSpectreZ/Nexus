project "glad"
	kind "SharedLib"
	language "C"
	location (vcxDir)

	targetdir (BinDir)
    objdir (IntDir)

    includedirs
    {
        IncludeDir["glad"]
    }

	files
	{
        (DepDir.."glad/include/**.h"),
        (DepDir.."glad/src/**.c"),
	}

    defines
    {
        "GLAD_GLAPI_EXPORT",
        "GLAD_GLAPI_EXPORT_BUILD"
    }

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
        defines "DEBUG"
        optimize "Off"
        symbols "Full"

    filter "configurations:Release"
        defines "RELEASE"
        optimize "Speed"
        symbols "FastLink"

    filter "configurations:Dist"
        defines "DIST"
        optimize "Full"
        symbols "Off"