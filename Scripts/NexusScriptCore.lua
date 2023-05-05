project "NexusScriptCore"
    kind "SharedLib"
    language "C#"
    dotnetframework "4.7.2"
    location (SrcDir.."%{prj.name}")

    targetdir(ScriptBinDir)
    objdir(ScriptIntDir)

    files
    {
        (SrcDir.."%{prj.name}/Source/**.cs"),
        (SrcDir.."%{prj.name}/Properties/**.cs"),
    }

	filter "configurations:Debug"
        optimize "Off"
        symbols "Full"

    filter "configurations:Release"
        optimize "Speed"
        symbols "FastLink"

    filter "configurations:Dist"
        optimize "Full"
        symbols "Off"