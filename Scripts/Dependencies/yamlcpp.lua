project "yamlcpp"
	kind "SharedLib"
	language "C++"
	location (vcxDir)

	targetdir (BinDir)
    objdir (IntDir)

	files
	{
        (DepDir.."yamlcpp/src/**.h"),
        (DepDir.."yamlcpp/src/**.cpp"),

        (DepDir.."yamlcpp/include/**.h")
	}

	includedirs
	{
		IncludePath["yamlcpp"]
	}

    defines 
    {
        "yaml_cpp_EXPORTS"
    }

    disablewarnings
    {
        "4275",
        "4251"
    }
    
	filter "system:windows"
		systemversion "latest"
		cppdialect "C++17"
		staticruntime "off"

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