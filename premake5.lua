include "Scripts/Path.lua"

workspace "Nexus-Engine"
    architecture "x64"

    configurations
    {
        "Debug","Release","Dist"
    }

    startproject "Launcher"

    group "Dependencies"
        include "Scripts/Dependencies/glfw.lua"
        include "Scripts/Dependencies/spdlog.lua"
        include "Scripts/Dependencies/imgui.lua"
        include "Scripts/Dependencies/yamlcpp.lua"
        include "Scripts/Dependencies/openALsoft.lua"
        include "Scripts/Dependencies/glad.lua"
    group ""

    group "Engine"
        include "Scripts/Engine/Platform.lua"
        include "Scripts/Engine/Utility.lua"
        include "Scripts/Engine/Graphics.lua"
    group ""

    group "Programs"
        include "Scripts/Programs/Launcher.lua"
    group ""