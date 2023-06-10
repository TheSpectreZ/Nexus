include "Scripts/Path.lua"

workspace "Nexus-Engine"
    architecture "x64"

    configurations
    {
        "Debug","Release","Dist"
    }

    startproject "NexusEd"

    group "Dependencies"
        include "Scripts/Dependencies/imgui.lua"
        include "Scripts/Dependencies/yamlcpp.lua"
        include "Scripts/Dependencies/jolt.lua"
    group ""

    group "Nexus"
        include "Scripts/Engine/Core.lua"
        include "Scripts/Engine/Application.lua"
        include "Scripts/Engine/Graphics.lua"
        include "Scripts/Engine/Renderer.lua"
    group ""
    
    group "Platform"
        include "Scripts/Engine/Vulkan.lua"
    group ""

    group "Tools"
        include "Scripts/Tools/NexusEd.lua"
    group ""