DepDir = "%{wks.location}/Source/Dependencies/"
VenDir = "%{wks.location}/Source/Vendor/"
SrcDir = "%{wks.location}/Source/"

BinDir = "%{wks.location}/Binaries/%{cfg.buildcfg}"

IntDir = "%{wks.location}/Intermediates/%{prj.name}/%{cfg.buildcfg}/"
vcxDir = "%{wks.location}/Intermediates/%{prj.name}"

IncludeDir = {}

IncludeDir["glfw"] = (DepDir.."glfw/include")
IncludeDir["spdlog"] = (DepDir.."spdlog/include")
IncludeDir["vulkanMemoryAllocator"] = (DepDir.."VulkanMemoryAllocator/include")
IncludeDir["stb"] = (DepDir.."stb")
IncludeDir["imgui"] = (DepDir.."imgui")
IncludeDir["entt"] = (DepDir.."entt/single_include")
IncludeDir["yamlcpp"] = (DepDir.."yamlcpp/include")
IncludeDir["glad"] = (DepDir.."glad/include")
IncludeDir["nlohmannJson"] = (DepDir.."nlohmannJson/single_include")
IncludeDir["assimp"] = (DepDir.."Assimp/include")
IncludeDir["openALsoft"] = (DepDir.."openALsoft/include")
IncludeDir["mono"] = (VenDir.."Mono/include")
IncludeDir["joltRoot"] = (DepDir.."Jolt")
IncludeDir["jolt"] = (DepDir.."Jolt/Jolt")
IncludeDir["tinygltf"] = (DepDir.."tinygltf")
IncludeDir["Nexus"] = (SrcDir.."Nexus/src/Nexus")

LibDir = {}

LibDir["vulkanSDK"] = "$(VULKAN_SDK)/Lib"