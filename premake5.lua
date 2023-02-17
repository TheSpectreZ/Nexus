include "Scripts/Path.lua"

workspace "Nexus-Engine"
    architecture "x64"

    configurations
    {
        "Debug","Release","Dist"
    }

    startproject "Sandbox"

    group "Dependencies"
        include "Scripts/Dependencies/glfw.lua"
        include "Scripts/Dependencies/spdlog.lua"
        include "Scripts/Dependencies/imgui.lua"
    group ""

    group "Engine"
        include "Scripts/Engine/Platform.lua"
        include "Scripts/Engine/DebugUtils.lua"
        include "Scripts/Engine/Graphics.lua"
        include "Scripts/Engine/UserInterface.lua"
        include "Scripts/Engine/Application.lua"
    group ""

    group "Programs"
        include "Scripts/Programs/Sandbox.lua"
    group ""
