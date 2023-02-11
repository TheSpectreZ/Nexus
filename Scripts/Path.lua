PrjDir = "%{wks.location}/Source/Projects/"
EngDir = "%{wks.location}/Source/Engine/"
DepDir = "%{wks.location}/Source/Dependencies/"

BinDir = "%{wks.location}/Binaries/%{cfg.buildcfg}"

IntDir = "%{wks.location}/Intermediates/%{prj.name}/%{cfg.buildcfg}/"
vcxDir = "%{wks.location}/Intermediates/%{prj.name}"

IncludeDir = {}

IncludeDir["glfw"] = (DepDir.."glfw/include")
IncludeDir["spdlog"] = (DepDir.."spdlog/include")
IncludeDir["vulkanSDK"] = "$(VULKAN_SDK)/Include"
IncludeDir["vulkanMemoryAllocator"] = (DepDir.."VulkanMemoryAllocator/include")
IncludeDir["stb"] = (DepDir.."stb")

IncludeDir["Platform"] = (EngDir.."Platform/include")
IncludeDir["DebugUtils"] = (EngDir.."DebugUtils/include")
IncludeDir["Graphics"] = (EngDir.."Graphics/include")
IncludeDir["Application"] = (EngDir.."Application/include")

LibDir = {}

LibDir["vulkanSDK"] = "$(VULKAN_SDK)/Lib"