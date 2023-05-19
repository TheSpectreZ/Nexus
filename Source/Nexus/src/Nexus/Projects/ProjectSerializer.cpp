#include "nxpch.h"
#include "ProjectSerializer.h"
#include <filesystem>
#include "yaml-cpp/yaml.h"

bool Nexus::ProjectSerializer::Serialize(const ProjectSpecifications& specs)
{
    YAML::Emitter out;
    out << YAML::BeginMap;

    out << YAML::Key << "Project" << YAML::Value << specs.Version;
    out << YAML::Key << "Name" << YAML::Value << specs.Name;
    out << YAML::Key << "Rootpath" << YAML::Value << specs.RootPath;

    out << YAML::Key << "Render Settings";
    out << YAML::BeginMap;
    out << YAML::Key << "Enable MultiSampling" << YAML::Value << specs.renderSettings.EnableMultiSampling;
    out << YAML::Key << "Enable HDR" << YAML::Value << specs.renderSettings.EnableHDR;
    out << YAML::EndMap;

    out << YAML::EndMap;

    std::string filepath = specs.RootPath + "\\" + specs.Name + ".nxProject";

    if (!std::filesystem::is_directory(specs.RootPath))
    {
        std::filesystem::create_directory(specs.RootPath);
    }

    std::ofstream fout(filepath);
    fout << out.c_str();

    return true;
}

bool Nexus::ProjectSerializer::DeSerialize(const std::string& path, ProjectSpecifications& specs)
{
    std::ifstream fin(path);

    std::stringstream stream;
    stream << fin.rdbuf();

    YAML::Node data = YAML::Load(stream.str());

    if (!data["Project"])
        return false;

    specs.Version = data["Project"].as<std::string>();
    specs.Name = data["Name"].as<std::string>();
    specs.RootPath = data["Rootpath"].as<std::string>();

    auto renderSettings = data["Render Settings"];

    if (!renderSettings)
        return false;

    specs.renderSettings.EnableMultiSampling = renderSettings["Enable MultiSampling"].as<bool>();
    specs.renderSettings.EnableHDR = renderSettings["Enable HDR"].as<bool>();

    return true;
}
