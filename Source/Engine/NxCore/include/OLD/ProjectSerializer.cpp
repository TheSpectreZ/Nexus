#include "NxCore/ProjectSerializer.h"
#include "nlohmann/json.hpp"    
#include <fstream>
#include <filesystem>

bool Nexus::ProjectSerializer::Serialize(const ProjectSpecifications& specs)
{
    nlohmann::json Json;
    Json["Project"] = specs.Version;
    Json["Name"] = specs.Name;
    Json["Path"] = specs.RootPath;

    nlohmann::json RenderSettings;
    RenderSettings["Enable_MSAA"] = specs.renderSettings.EnableMultiSampling;
    RenderSettings["Enable_HDR"] = specs.renderSettings.EnableHDR;

    Json["Render_Settings"] = RenderSettings;
    
    std::string JsonDump = Json.dump(4);

    std::string filepath = specs.RootPath + "\\" + specs.Name + ".NxProj";

    if (!std::filesystem::is_directory(specs.RootPath))
    {
        std::filesystem::create_directory(specs.RootPath);
    }

    std::ofstream fout(filepath);
    fout << JsonDump.c_str();

    return true;
}

bool Nexus::ProjectSerializer::DeSerialize(const std::string& path, ProjectSpecifications& specs)
{
    std::ifstream fin(path);

    std::stringstream stream;
    stream << fin.rdbuf();

    nlohmann::json data = nlohmann::json::parse(stream.str());
  
    if (!data.contains("Project"))
        return false;
    
    specs.Version = data["Project"].get<std::string>();
    specs.Name = data["Name"].get<std::string>();
    specs.RootPath = data["Path"].get<std::string>();

    auto& renderSettings = data["Render_Settings"];

    if (renderSettings.empty())
        return false;

    specs.renderSettings.EnableMultiSampling = renderSettings["Enable_MSAA"].get<bool>();
    specs.renderSettings.EnableHDR = renderSettings["Enable_HDR"].get<bool>();

    return true;
}
