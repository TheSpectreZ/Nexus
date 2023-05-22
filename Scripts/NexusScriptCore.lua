project "NexusScriptCore"
    kind "SharedLib"
    language "C#"
    dotnetframework "4.7.2"
    location "../Source/%{prj.name}"

    targetdir "../Source/NexusEditor/Resources/Scripts"
    objdir "../Source/NexusEditor/Resources/Scripts/Int/%{prj.name}"

    files
    {
        "../Source/%{prj.name}/Source/**.cs",
        "../Source/%{prj.name}/Properties/**.cs",
    }

	filter "configurations:Debug"
        optimize "Off"
        symbols "Full"

    filter "configurations:Release"
        optimize "Speed"
        symbols "FastLink"

    filter "configurations:Dist"
        optimize "Full"
        symbols "Off"