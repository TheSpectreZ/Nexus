include "Scripts/Path.lua"

workspace "Nexus-Engine"
    architecture "x64"

    configurations
    {
        "Debug","Release","Dist"
    }

    startproject "NxEditor"

    group "Dependencies"
        include "Scripts/Dependencies/imgui.lua"
        include "Scripts/Dependencies/glfw.lua"
        include "Scripts/Dependencies/utils.lua"
    group ""

    group "Nexus"
        include "Scripts/Engine/Core.lua"
        include "Scripts/Engine/Application.lua"
        include "Scripts/Engine/Scene.lua"
        include "Scripts/Engine/Graphics.lua"
        include "Scripts/Engine/RenderEngine.lua"
        include "Scripts/Engine/PhysicsEngine.lua"
        include "Scripts/Engine/ScriptEngine.lua"
        include "Scripts/Engine/Asset.lua"
    group ""
        
    group "Script-Core"
        include "Scripts/NexusScriptCore.lua"
    group ""
        
    group "Platform"
        include "Scripts/Engine/Vulkan.lua"
        include "Scripts/Engine/DirectX.lua"
    group ""

    group "Tools"
        include "Scripts/Tools/NxImGui.lua"
        include "Scripts/Tools/NxEditor.lua"
        include "Scripts/Tools/NxLauncher.lua"
    group ""