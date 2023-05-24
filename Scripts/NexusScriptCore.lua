local RootPath = os.getenv("NEXUS_ROOT_PATH")
assert(RootPath ~= nil, "Environment variable PATH not set")

project "NexusScriptCore"
    kind "SharedLib"
    language "C#"
    dotnetframework "4.7.2"
    location (RootPath.."/Source/%{prj.name}")

    targetdir (RootPath.."/Source/NexusEditor/Resources/Scripts")
    objdir (RootPath.."/Source/NexusEditor/Resources/Scripts/Int/%{prj.name}")

    files
    {
        (RootPath.."/Source/%{prj.name}/Source/**.cs"),
        (RootPath.."/Source/%{prj.name}/Properties/**.cs"),
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