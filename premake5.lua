include "Scripts/Path.lua"

workspace "Nexus-Engine"
    architecture "x64"

    configurations
    {
        "Debug","Release","Dist"
    }

    group "Dependencies"
        include "Scripts/Dependencies/glfw.lua"
        include "Scripts/Dependencies/spdlog.lua"
    group ""

    group "Engine"
        include "Scripts/Engine/Platform.lua"
        include "Scripts/Engine/DebugUtils.lua"
    group ""

    group "Applications"
        include "Scripts/Applications/Sandbox.lua"
    group ""
