workspace "Sandbox"
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
        location ("Scripts")
        
        targetdir "Scripts/Bin/%{cfg.buildcfg}"
        objdir "Scripts/Bin-Int/%{cfg.buildcfg}"

        files
        {
            "Scripts/Source/**.cs",
            "Scripts/Properties/**.cs"
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

    local path = os.getenv("NEXUS_ROOT_PATH")
    assert(path ~= nil, "Environment variable PATH not set")

    group "Nexus"
        include (path.."/Scripts/NexusScriptCore.lua")
    group ""
