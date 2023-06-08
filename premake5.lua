include "Scripts/Path.lua"

workspace "Nexus-Engine"
    architecture "x64"

    configurations
    {
        "Debug","Release","Dist"
    }

    startproject "NexusEd"

    group "Dependencies"
        include "Scripts/Dependencies/glfw.lua"
        include "Scripts/Dependencies/spdlog.lua"
        include "Scripts/Dependencies/imgui.lua"
        include "Scripts/Dependencies/yamlcpp.lua"
        include "Scripts/Dependencies/glad.lua"
        include "Scripts/Dependencies/jolt.lua"
    
    group "NexusEngine"
        include "Scripts/Engine/Core.lua"
        include "Scripts/Engine/Engine.lua"
    
    group "Tools"
        include "Scripts/Tools/NexusEd.lua"