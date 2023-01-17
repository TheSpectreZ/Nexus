project "spdlog"
    kind "StaticLib"
	language "C++"
	location (vcxDir)

	targetdir (BinDir)
    objdir (IntDir)

	includedirs
	{
		(DepDir.."spdlog/include")
	}

	files
    {
        (DepDir.."spdlog/src/spdlog.cpp"),
        (DepDir.."spdlog/src/stdout_sinks.cpp"),
        (DepDir.."spdlog/src/color_sinks.cpp"),
        (DepDir.."spdlog/src/file_sinks.cpp"),
        (DepDir.."spdlog/src/async.cpp"),
        (DepDir.."spdlog/src/cfg.cpp"),
    }

	defines "SPDLOG_COMPILED_LIB"

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++20"

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