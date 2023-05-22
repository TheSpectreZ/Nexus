workspace "@ProjectName@"
    architecture "x64"
    startproject "@ProjectName@"

    configurations
    {
        "Debug","Release","Dist"
    }

    project "@ProjectName@"
        kind "SharedLib"
        language "C#"
        dotnetframework "4.7.2"
        location ("@ProjectName@")
        
        targetdir "../Resources/Scripts"
        objdir "../Resources/Scripts/Int/%{prj.name}"

        files
        {
            "@ProjectName@/Source/**.cs",
            "@ProjectName@/Properties/**.cs"
        }

        links
        {
            "NexusScriptCore"
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

    group "Nexus"
        include "../../../Scripts/NexusScriptCore.lua"
    group ""