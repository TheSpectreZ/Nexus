PrmDir = "%{wks.location}/Source/Programs/"
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
IncludeDir["imgui"] = (DepDir.."imgui")
IncludeDir["entt"] = (DepDir.."entt/single_include")
IncludeDir["yamlcpp"] = (DepDir.."yamlcpp/include")
IncludeDir["glad"] = (DepDir.."glad/include")
IncludeDir["nlohmannJson"] = (DepDir.."nlohmannJson/single_include")
IncludeDir["openALsoft"] = (DepDir.."openALsoft/include")

IncludeDir["Platform"] = (EngDir.."Platform/include")
IncludeDir["Utility"] = (EngDir.."Utility/include")
IncludeDir["Graphics"] = (EngDir.."Graphics/include")

LibDir = {}

LibDir["vulkanSDK"] = "$(VULKAN_SDK)/Lib"