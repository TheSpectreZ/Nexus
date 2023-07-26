DepDir = "%{wks.location}/Source/Dependencies/"
VenDir = "%{wks.location}/Source/Vendor/"
SrcDir = "%{wks.location}/Source/"

BinDir = "%{wks.location}/Binaries/%{cfg.buildcfg}"

IntDir = "%{wks.location}/Intermediates/%{prj.name}/%{cfg.buildcfg}/"
vcxDir = "%{wks.location}/Intermediates/%{prj.name}"

IncludePath = {}

IncludePath["imgui"] = (DepDir.."imgui")
IncludePath["glfw"] = (DepDir.."glfw/include")
IncludePath["utils"] = (DepDir.."utils/include")

IncludePath["mono"] = (VenDir.."Mono/include")
IncludePath["physx"] = (VenDir.."PhysX/include")

IncludePath["NxCore"] = (SrcDir.."Engine/NxCore/include")
IncludePath["NxScene"] = (SrcDir.."Engine/NxScene/include")
IncludePath["NxAsset"] = (SrcDir.."Engine/NxAsset/include")
IncludePath["NxVulkan"] = (SrcDir.."Engine/NxVulkan/include")
IncludePath["NxGraphics"] = (SrcDir.."Engine/NxGraphics/include")
IncludePath["NxRenderEngine"] = (SrcDir.."Engine/NxRenderEngine/include")
IncludePath["NxPhysicsEngine"] = (SrcDir.."Engine/NxPhysicsEngine/include")
IncludePath["NxScriptEngine"] = (SrcDir.."Engine/NxScriptEngine/include")
IncludePath["NxApplication"] = (SrcDir.."Engine/NxApplication/include")

IncludePath["NxImGui"] = (SrcDir.."Tools/NxImGui/include")