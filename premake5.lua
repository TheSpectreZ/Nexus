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
        include "Scripts/Engine/GameEngine.lua"
        include "Scripts/Engine/Scene.lua"
        include "Scripts/Engine/Asset.lua"
        include "Scripts/Engine/Graphics.lua"
        include "Scripts/Engine/RenderEngine.lua"
        include "Scripts/Engine/PhysicsEngine.lua"
        include "Scripts/Engine/ScriptEngine.lua"
        include "Scripts/Engine/NetworkEngine.lua"
        include "Scripts/Engine/ShaderCompiler.lua"
        include "Scripts/Engine/UserInterface.lua"
    group ""

    group "Plugins"
        include "Scripts/Engine/Vulkan.lua"
    group ""

    include "Scripts/Tools/NxEditor.lua"
    include "Scripts/Misc.lua"