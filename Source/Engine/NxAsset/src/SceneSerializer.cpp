#include "NxAsset/SceneSerializer.h"

#include "nlohmann/json.hpp"

bool Nexus::Serializer::SaveAsJson(Nexus::Ref<Scene> scene,const std::string& assetPath,const std::string& name)
{
    nlohmann::json Json;

    // Root
    {
        nlohmann::json root;
    }

    // Entities
    nlohmann::json entities;
    {
    }

    Json["Entities"] = entities;

    std::string JsonDump = Json.dump(4);

    std::string Name = material->Name;
    if (Name.empty())
        Name = std::to_string(uid.operator size_t());

    AssetFilePath file = AssetFilePath(dstFolder.generic_string() + "/" + Name + MaterialExtension);

    if (std::filesystem::exists(file))
        file.replace_filename(std::string(Name + "- Copy" + MaterialExtension));

    std::ofstream stream(file);
    stream << JsonDump;
}
