project "jolt"
	kind "SharedLib"
	language "C++"
    cppdialect "C++17"
	location (vcxDir)
    
	targetdir (BinDir)
    objdir (IntDir)

    includedirs
    {
        IncludePath["jolt"],
        IncludePath["joltRoot"],
    }

	files
	{
        (DepDir.."Jolt/Jolt/**.h"),
        (DepDir.."Jolt/Jolt/**.cpp"),
        (DepDir.."Jolt/Jolt/**.inl"),
        (DepDir.."Jolt/Jolt/**.gliffy"),
	}
    
    defines
    {
        "JPH_SHARED_LIBRARY",
        "JPH_BUILD_SHARED_LIBRARY",
    }

	filter "system:windows"
		systemversion "latest"
        files { (DepDir.."Jolt/Jolt/Jolt.natvis") }

	filter "configurations:Debug"
        optimize "Off"
        symbols "Full"
        defines 
        {
            "_DEBUG",
            "JPH_DEBUG_RENDERER",
            "JPH_FLOATING_POINT_EXCEPTIONS_ENABLED",
            "JPH_PROFILE_ENABLED"
        }

    filter "configurations:Release"
        optimize "Speed"
        symbols "FastLink"
        vectorextensions "AVX2"
        isaextensions { "BMI", "POPCNT", "LZCNT", "F16C" }

        defines
        {
            "RELEASE",
            "JPH_DEBUG_RENDERER",
            "JPH_FLOATING_POINT_EXCEPTIONS_ENABLED",
            "JPH_PROFILE_ENABLED"
        }

    filter "configurations:Dist"
        defines "DIST"
        optimize "Full"
        symbols "Off"
        vectorextensions "AVX2"
        isaextensions { "BMI", "POPCNT", "LZCNT", "F16C" }