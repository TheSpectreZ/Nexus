include "Scripts/Path.lua"

workspace "Nexus-Engine"
    architecture "x64"

    configurations
    {
        "Debug","Release","Dist"
    }

    group "Dependencies"
    group ""

    group "Engine"
    group ""

    group "Applications"
        include "Scripts/Applications/Sandbox.lua"
    group ""
