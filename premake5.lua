include "Scripts/Path.lua"

workspace "Nexus-Engine"
    architecture "x64"

    configurations
    {
        "Debug","Release","Dist"
    }

    startproject "Minecraft"

    group "Dependencies"
        include "Scripts/Dependencies/glfw.lua"
        include "Scripts/Dependencies/spdlog.lua"
    group ""

    group "Engine"
        include "Scripts/Engine/Platform.lua"
        include "Scripts/Engine/DebugUtils.lua"
        include "Scripts/Engine/Graphics.lua"
        include "Scripts/Engine/Application.lua"
    group ""

    group "Projects"
        
    group ""
