DepDir = "%{wks.location}/Source/Dependencies/"
VenDir = "%{wks.location}/Source/Vendor/"
SrcDir = "%{wks.location}/Source/"

BinDir = "%{wks.location}/Binaries/%{cfg.buildcfg}"

IntDir = "%{wks.location}/Intermediates/%{prj.name}/%{cfg.buildcfg}/"
vcxDir = "%{wks.location}/Intermediates/%{prj.name}"

IncludePath = {}
IncludePath["VMA"] = (DepDir.."VulkanMemoryAllocator/include")
IncludePath["stb"] = (DepDir.."stb")
IncludePath["imgui"] = (DepDir.."imgui")
IncludePath["entt"] = (DepDir.."entt/single_include")
IncludePath["yamlcpp"] = (DepDir.."yamlcpp/include")
IncludePath["nlohmannJson"] = (DepDir.."nlohmannJson/single_include")
IncludePath["mono"] = (VenDir.."Mono/include")
IncludePath["joltRoot"] = (DepDir.."Jolt")
IncludePath["jolt"] = (DepDir.."Jolt/Jolt")
IncludePath["tinygltf"] = (DepDir.."tinygltf")
IncludePath["filewatch"] = (DepDir.."filewatch")
IncludePath["glfw"] = (DepDir.."glfw/include")

IncludePath["NxCore"] = (SrcDir.."Engine/NxCore/include")
IncludePath["NxScene"] = (SrcDir.."Engine/NxScene/include")
IncludePath["NxVulkan"] = (SrcDir.."Engine/NxVulkan/include")
IncludePath["NxGraphics"] = (SrcDir.."Engine/NxGraphics/include")
IncludePath["NxRenderer"] = (SrcDir.."Engine/NxRenderer/include")
IncludePath["NxApplication"] = (SrcDir.."Engine/NxApplication/include")