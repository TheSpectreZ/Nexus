workspace "SandboxProject"
    architecture "x64"
    startproject "Sandbox"

    configurations
    {
        "Debug","Release","Dist"
    }

    project "Sandbox"
        kind "SharedLib"
        language "C#"
        dotnetframework "4.7.2"
        location("Sandbox")
        
        targetdir "../Resources/Scripts"
        objdir "../Resources/Scripts/Int/%{prj.name}"

        files
        {
            "Sandbox/Source/**.cs",
            "Sandbox/Properties/**.cs"
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