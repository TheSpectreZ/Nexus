#include "nxpch.h"
#include "SceneSerializer.h"
#include "Scene/Entity.h"
#include "yaml-cpp/yaml.h"
#include  "Assets/AssetManager.h"

template<>
struct YAML::convert<glm::vec3>
{
	static Node encode(const glm::vec3& v)
	{
		Node node;
		node.push_back(v.x);
		node.push_back(v.y);
		node.push_back(v.z);
		node.SetStyle(EmitterStyle::Flow);
		return node;
	}

	static bool decode(const Node& node, glm::vec3& v)
	{
		if (!node.IsSequence() || node.size() != 3)
			return false;

		v.x = node[0].as<float>();
		v.y = node[1].as<float>();
		v.z = node[2].as<float>();

		return true;
	}
};


YAML::Emitter& operator <<(YAML::Emitter& emmiter, const glm::vec3 vector)
{
	emmiter << YAML::Flow;
	emmiter << YAML::BeginSeq << vector.x << vector.y << vector.z << YAML::EndSeq;
	return emmiter;
}

void SerializeEntity(YAML::Emitter& out, Nexus::Entity e)
{
	using namespace Nexus;
	
	out << YAML::BeginMap;

	// Identity
	auto& IdComponent = e.GetComponent<Component::Identity>();
	out << YAML::Key << "UUID" << YAML::Value << IdComponent.uuid;

	// Tag
	{
		auto& TagComponent = e.GetComponent<Component::Tag>();
		out << YAML::Key << "TagComponent";
		out << YAML::BeginMap;
		out << YAML::Key << "Name" << YAML::Value << TagComponent.name;
		out << YAML::EndMap;
	}

	// Transform
	{
		auto& t = e.GetComponent<Component::Transform>();

		out << YAML::Key << "TransformComponent";
		out << YAML::BeginMap;
		out << YAML::Key << "Translation" << YAML::Value << t.Translation;
		out << YAML::Key << "Rotation" << YAML::Value << t.GetRotationEuler();
		out << YAML::Key << "Scale" << YAML::Value << t.Scale;
		out << YAML::EndMap;
	}

	//// Static Mesh
	//if (e.HasComponent<Component::Mesh>())
	//{
	//	auto& m = e.GetComponent<Component::Mesh>();
	//	auto& asset = Nexus::AssetManager::Get<StaticMesh>(m.handle);
	//
	//	out << YAML::Key << "MeshComponent";
	//	out << YAML::BeginMap;
	//	out << YAML::Key << "Name" << YAML::Value << asset.Name;
	//	out << YAML::Key << "Source" << YAML::Value << asset.Path.string();
	//	out << YAML::EndMap;
	//}

	out << YAML::EndMap;
}

void Nexus::SceneSerializer::Serialize(Ref<Scene> scene, const std::string& filepath)
{
	YAML::Emitter out;

	out << YAML::BeginMap;
	out << YAML::Key << "Scene" << YAML::Value << "Untitled";

	out << YAML::Key << "Entities";
	out << YAML::Value << YAML::BeginSeq;

	scene->m_registry.each([&](entt::entity e)
		{
			if (e == entt::null)
				return;

			Entity en(e, scene.get());
			SerializeEntity(out, en);
		});
	
	out << YAML::EndSeq;
	out << YAML::EndMap;

	std::ofstream fout(filepath);
	fout << out.c_str();
}

bool Nexus::SceneSerializer::Deserialize(Ref<Scene> scene, const std::string& filepath)
{
	std::ifstream fin(filepath);

	std::stringstream stream;
	stream << fin.rdbuf();

	YAML::Node data = YAML::Load(stream.str());

	if (!data["Scene"])
		return false;
	
	auto entities = data["Entities"];

	if (entities)
	{
		for (auto entity : entities)
		{
			auto uuid = entity["UUID"].as<uint64_t>();

			auto TagComponent = entity["TagComponent"];
			auto Name = TagComponent["Name"].as<std::string>();

			Entity e = scene->CreateEntity(Name, uuid);

			auto TransformComponent = entity["TransformComponent"];
			if (TransformComponent)
			{
				glm::vec3 t = TransformComponent["Translation"].as<glm::vec3>();
				glm::vec3 r = TransformComponent["Rotation"].as<glm::vec3>();
				glm::vec3 s = TransformComponent["Scale"].as<glm::vec3>();

				auto& transform = e.GetComponent<Component::Transform>();
				transform.Translation = t;
				transform.Scale = s;
				transform.SetRotationEuler(r);
			}

			//auto MeshComponent = entity["MeshComponent"];
			//if (MeshComponent)
			//{
			//	auto name = MeshComponent["Name"].as<std::string>();
			//	auto path = MeshComponent["Source"].as<std::string>();
			//
			//	auto& mesh = e.GetComponent<Component::Mesh>();
			//
			//}
		}
	}
	
	return true;
}
