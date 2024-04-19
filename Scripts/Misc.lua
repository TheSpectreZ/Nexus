group "Build"

    project "Scripts"
        kind "Utility"
        files 
        { 
            "**.lua",
            "%{wks.location}/premake5.lua"
        }

group ""

group "Assets"

    project "Shaders"
        kind "Utility"
        location "%{wks.location}/Assets/Shaders"

        files 
        {
            "%{wks.location}/Assets/Shaders/**.glsl"
        }

group ""
