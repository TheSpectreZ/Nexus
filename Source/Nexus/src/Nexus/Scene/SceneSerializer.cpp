#include "nxpch.h"
#include "SceneSerializer.h"
#include "Scene/Entity.h"
#include "yaml-cpp/yaml.h"

#include "Assets/Importer/glTFImporter.h"
#include "Assets/AssetManager.h"
#include "Renderer/Mesh.h"
#include "Renderer/Texture.h"
#include "Scene/Material.h"

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

static const char* GetRigidBodyMotionTypeName(Nexus::Component::RigidBody::MotionType type)
{
	switch (type)
	{
	case Nexus::Component::RigidBody::MotionType::Static:
		return "Static";
	case Nexus::Component::RigidBody::MotionType::Dynamic:
		return "Dynamic";
	case Nexus::Component::RigidBody::MotionType::Kinematic:
		return "Kinematic";
	default:
		return "";
	}
}

static Nexus::Component::RigidBody::MotionType GetRigidBodyMotionTypeFromName(const std::string& name)
{
	if (name == "Dynamic")
		return Nexus::Component::RigidBody::MotionType::Dynamic;
	else if (name == "Kinematic")
		return Nexus::Component::RigidBody::MotionType::Kinematic;
	else
		return Nexus::Component::RigidBody::MotionType::Static;
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

	// Mesh
	if(e.HasComponent<Component::Mesh>())
	{
		auto& m = e.GetComponent<Component::Mesh>();

		auto mesh = AssetManager::Get<StaticMesh>(m.handle);

		out << YAML::Key << "MeshComponent";
		out << YAML::BeginMap;
		out << YAML::Key << "Path" << mesh->GetPath();
		out << YAML::EndMap;
	}

	// Script
	if (e.HasComponent<Component::Script>())
	{
		auto& s = e.GetComponent<Component::Script>();

		out << YAML::Key << "ScriptComponent";
		out << YAML::BeginMap;
		out << YAML::Key << "ScriptClassName" << YAML::Value << s.name;
		out << YAML::EndMap;	
	}

	// Rigidbody
	if (e.HasComponent<Component::RigidBody>())
	{
		auto& r = e.GetComponent<Component::RigidBody>();

		out << YAML::Key << "RigidBodyComponent";
		out << YAML::BeginMap;
		out << YAML::Key << "MotionType" << YAML::Value << GetRigidBodyMotionTypeName(r.motionType);
		out << YAML::Key << "Mass" << YAML::Value << r.mass;
		out << YAML::Key << "Friction" << YAML::Value << r.friction;
		out << YAML::Key << "Restitution" << YAML::Value << r.restitution;
		out << YAML::EndMap;
	}

	// BoxCollider
	if (e.HasComponent<Component::BoxCollider>())
	{
		auto& b = e.GetComponent<Component::BoxCollider>();

		out << YAML::Key << "BoxColliderComponent";
		out << YAML::BeginMap;
		out << YAML::Key << "HalfExtent" << YAML::Value << b.HalfExtent;
		out << YAML::EndMap;
	}

	// SphereCollider
	if (e.HasComponent<Component::SphereCollider>())
	{
		auto& s = e.GetComponent<Component::SphereCollider>();

		out << YAML::Key << "SphereColliderComponent";
		out << YAML::BeginMap;
		out << YAML::Key << "Radius" << YAML::Value << s.Radius;
		out << YAML::EndMap;
	}

	// CapsuleCollider
	if (e.HasComponent<Component::CapsuleCollider>())
	{
		auto& c = e.GetComponent<Component::CapsuleCollider>();

		out << YAML::Key << "CapsuleColliderComponent";
		out << YAML::BeginMap;
		out << YAML::Key << "HalfHeight" << YAML::Value << c.HalfHeight;
		out << YAML::Key << "TopRadius" << YAML::Value << c.TopRadius;
		out << YAML::Key << "BottomRadius" << YAML::Value << c.BottomRadius;
		out << YAML::EndMap;
	}

	// DirectionalLight
	if (e.HasComponent<Component::DirectionalLight>())
	{
		auto& d = e.GetComponent<Component::DirectionalLight>();

		out << YAML::Key << "DirectionalLightComponent";
		out << YAML::BeginMap;
		out << YAML::Key << "Direction" << YAML::Value << d.direction;
		out << YAML::Key << "Color" << YAML::Value << d.color;
		out << YAML::EndMap;
	}

	// PointLight
	if (e.HasComponent<Component::PointLight>())
	{
		auto& p = e.GetComponent<Component::PointLight>();

		out << YAML::Key << "PointLightComponent";
		out << YAML::BeginMap;
		out << YAML::Key << "Color" << p.color;
		out << YAML::Key << "Size" << p.size;
		out << YAML::Key << "Intensity" << p.intensity;
		out << YAML::EndMap;
	}

	out << YAML::EndMap;
}

void Nexus::SceneSerializer::Serialize(Ref<Scene> scene, Ref<PhysicsWorld> physics, const std::string& filepath)
{
	YAML::Emitter out;

	out << YAML::BeginMap;
	out << YAML::Key << "Scene" << YAML::Value << "Untitled";

	out << YAML::Key << "Physics";
	out << YAML::BeginMap;
	out << YAML::Key << "Gravity" << physics->GetGravity();
	out << YAML::EndMap;

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

bool Nexus::SceneSerializer::Deserialize(Scene* scene, PhysicsWorld* physics, const std::string& filepath)
{
	std::ifstream fin(filepath);

	std::stringstream stream;
	stream << fin.rdbuf();

	YAML::Node data = YAML::Load(stream.str());

	if (!data["Scene"])
		return false;
	
	auto Physics = data["Physics"];

	if (Physics)
	{
		auto Gravity = Physics["Gravity"].as<glm::vec3>();
		physics->SetGravity(Gravity);
	}

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

			auto MeshComponent = entity["MeshComponent"];
			if (MeshComponent)
			{
				std::string path = MeshComponent["Path"].as<std::string>();

				auto [m, Id] = AssetManager::Load<StaticMesh>(path, nullptr);

				auto& mesh = e.AddComponent<Component::Mesh>();
				mesh.handle = Id;
			}

			auto ScriptComponent = entity["ScriptComponent"];
			if (ScriptComponent)
			{
				auto& script = e.AddComponent<Component::Script>();
				script.name = ScriptComponent["ScriptClassName"].as<std::string>();
			}

			auto RigidBodyComponent = entity["RigidBodyComponent"];
			if (RigidBodyComponent)
			{
				auto& r = e.AddComponent<Component::RigidBody>();
				r.motionType = GetRigidBodyMotionTypeFromName(RigidBodyComponent["MotionType"].as<std::string>());
				r.mass = RigidBodyComponent["Mass"].as<float>();
				r.friction = RigidBodyComponent["Friction"].as<float>();
				r.restitution = RigidBodyComponent["Restitution"].as<float>();
			}

			auto BoxColliderComponent = entity["BoxColliderComponent"];
			if (BoxColliderComponent)
			{
				auto& b = e.AddComponent<Component::BoxCollider>();
				b.HalfExtent = BoxColliderComponent["HalfExtent"].as<glm::vec3>();
			}

			auto SphereColliderComponent = entity["SphereColliderComponent"];
			if (SphereColliderComponent)
			{
				auto& b = e.AddComponent<Component::SphereCollider>();
				b.Radius = SphereColliderComponent["Radius"].as<float>();
			}

			auto CapsuleColliderComponent = entity["CapsuleColliderComponent"];
			if (CapsuleColliderComponent)
			{
				auto& c = e.AddComponent<Component::CapsuleCollider>();
				c.HalfHeight = CapsuleColliderComponent["HalfHeight"].as<float>();
				c.TopRadius = CapsuleColliderComponent["TopRadius"].as<float>();
				c.BottomRadius = CapsuleColliderComponent["BottomRadius"].as<float>();
			}

			auto DirectionalLightComponent = entity["DirectionalLightComponent"];
			if (DirectionalLightComponent)
			{
				auto& d = e.AddComponent<Component::DirectionalLight>();
				d.direction = DirectionalLightComponent["Direction"].as<glm::vec3>();
				d.color = DirectionalLightComponent["Color"].as<glm::vec3>();
			}

			auto PointLightComponent = entity["PointLightComponent"];
			if (PointLightComponent)
			{
				auto& p = e.AddComponent<Component::PointLight>();
				p.color = PointLightComponent["Color"].as<glm::vec3>();
				p.size = PointLightComponent["Size"].as<float>();
				p.intensity = PointLightComponent["Intensity"].as<float>();
			}
		}
	}
	
	return true;
}
