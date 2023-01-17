AppDir = "%{wks.location}/Source/Applications/"
EngDir = "%{wks.location}/Source/Engine/"
DepDir = "%{wks.location}/Source/Dependencies/"

BinDir = "%{wks.location}/Binaries/%{cfg.buildcfg}"

IntDir = "%{wks.location}/Intermediates/%{prj.name}/%{cfg.buildcfg}/"
vcxDir = "%{wks.location}/Intermediates/%{prj.name}"

IncludeDir = {}

IncludeDir["glfw"] = (DepDir.."glfw/include")
IncludeDir["spdlog"] = (DepDir.."spdlog/include")

IncludeDir["Platform"] = (EngDir.."Platform/include")
IncludeDir["DebugUtils"] = (EngDir.."DebugUtils/include")
