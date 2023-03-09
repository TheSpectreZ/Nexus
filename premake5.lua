include "Scripts/Path.lua"

workspace "Nexus-Engine"
    architecture "x64"

    configurations
    {
        "Debug","Release","Dist"
    }

    startproject "NexusEditor"

    group "Dependencies"
        include "Scripts/Dependencies/glfw.lua"
        include "Scripts/Dependencies/spdlog.lua"
        include "Scripts/Dependencies/imgui.lua"
        include "Scripts/Dependencies/yamlcpp.lua"
        include "Scripts/Dependencies/openALsoft.lua"
        include "Scripts/Dependencies/glad.lua"
    group ""

    include "Scripts/Nexus.lua"
    include "Scripts/NexusEditor.lua"