project "openALSoft"
	kind "StaticLib"
	language "C++"
	cppdialect "C++14"
	
	location (vcxDir)

	targetdir (BinDir)
    objdir (IntDir)

	includedirs
	{
		(DepDir.."openALSoft/src"),
		(DepDir.."openALSoft/src/alc"),
		(DepDir.."openALSoft/src/common"),
		(DepDir.."openALSoft/include"),
		(DepDir.."openALSoft/include/AL"),
	}

	files
	{
		(DepDir.."openALSoft/src/**.h"),
		(DepDir.."openALSoft/src/**.cpp"),
	}
	
	excludes
	{
		(DepDir.."openALSoft/src/alc/mixer/mixer_neon.cpp")
	}

	defines
	{
		"AL_LIBTYPE_STATIC"
	}

	filter "system:windows"
		systemversion "latest"
		
		disablewarnings 
		{
			"5030",
			"4065",
			"4834",
			"4858"
		}

		defines
		{
			"WIN32",
			"_WINDOWS",
			"AL_BUILD_LIBRARY",
			"AL_ALEXT_PROTOTYPES",
			"_WIN32",
			"_WIN32_WINNT=0x0502",
			"_CRT_SECURE_NO_WARNINGS",
			"NOMINMAX",
			"CMAKE_INTDIR=\"Debug\"",
			"OpenAL_EXPORTS"
		}

		links
		{
			"winmm"
		}

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