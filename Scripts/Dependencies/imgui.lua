project "imgui"
	kind "StaticLib"
	language "C++"
	location (vcxDir)
    staticruntime "Off"

	targetdir (BinDir)
    objdir (IntDir)

	files
	{
		(DepDir.."imgui/imconfig.h"),
		(DepDir.."imgui/imgui.h"),
		(DepDir.."imgui/imgui.cpp"),
		(DepDir.."imgui/imgui_draw.cpp"),
		(DepDir.."imgui/imgui_internal.h"),
		(DepDir.."imgui/imgui_tables.cpp"),
		(DepDir.."imgui/imgui_widgets.cpp"),
		(DepDir.."imgui/imstb_rectpack.h"),
		(DepDir.."imgui/imstb_textedit.h"),
		(DepDir.."imgui/imstb_truetype.h"),
		(DepDir.."imgui/imgui_demo.cpp"),

		(DepDir.."imgui/backends/imgui_impl_glfw.h"),
		(DepDir.."imgui/backends/imgui_impl_glfw.cpp"),

        (DepDir.."imgui/backends/imgui_impl_opengl3.h"),
		(DepDir.."imgui/backends/imgui_impl_opengl3.cpp"),

        (DepDir.."imgui/backends/imgui_impl_vulkan.h"),
		(DepDir.."imgui/backends/imgui_impl_vulkan.cpp"),
	}

	includedirs
	{
		IncludeDir["imgui"],
		IncludeDir["glfw"],
		
		"$(VULKAN_SDK)/Include"
	}

	links
	{
		"glfw",
		"vulkan-1.lib"
	}

	libdirs
	{
		LibDir["vulkanSDK"]
	}

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++17"

	filter "system:linux"
		pic "On"
		systemversion "latest"
		cppdialect "C++17"

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
