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
        include "Scripts/Dependencies/imgui.lua"
        include "Scripts/Dependencies/openalSoft.lua"
    group ""

    group "Engine"
        include "Scripts/Engine/Platform.lua"
        include "Scripts/Engine/DebugUtils.lua"
        include "Scripts/Engine/Graphics.lua"
        include "Scripts/Engine/Audio.lua"
        include "Scripts/Engine/UserInterface.lua"
        include "Scripts/Engine/Application.lua"
    group ""