project "utils"
	kind "Utility"
    language "C++"
	location (vcxDir)

	files
	{
		(DepDir.."utils/include/**.h"),
		(DepDir.."utils/include/**.hpp"),
	}

	filter "system:windows"
		systemversion "latest"
		
    filter "configurations:Debug"
        runtime "Debug"
        defines "DEBUG"
        optimize "Off"
        symbols "Full"

    filter "configurations:Release"
        runtime "Release"
        defines "RELEASE"
        optimize "Speed"
        symbols "FastLink"

    filter "configurations:Dist"
        runtime "Release"    
        defines "DIST"
        optimize "Full"
        symbols "Off"
