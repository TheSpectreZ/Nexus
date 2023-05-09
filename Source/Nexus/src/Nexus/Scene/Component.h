#pragma once
#include "Assets/Asset.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include <glm/gtx/quaternion.hpp>
#include "glm/gtc/type_ptr.hpp"

namespace Nexus
{
	typedef uint64_t UUID;
	UUID CreateUUID();

	namespace Component
	{
		struct Tag
		{
			Tag() = default;
			Tag(const Tag&) = default;
			Tag(const std::string& Name);
			
			std::string name;
		};

		struct Identity
		{
			Identity();
			Identity(const Identity&) = default;
			Identity(UUID id)
				:uuid(id)
			{}

			UUID uuid;
		};

		struct Transform
		{
			Transform() = default;
			Transform(const Transform&) = default;
			Transform(const glm::vec3 translation);
				
			glm::vec3 Translation = { 0.f,0.f,0.f };
			glm::vec3 Scale = { 1.f,1.f,1.f };

			glm::mat4 GetTransform() const;
			glm::quat GetRotation() const;
			glm::vec3 GetRotationEuler() const;

			void SetTransform(const glm::mat4& transform);
			void SetRotation(const glm::quat& quat);
			void SetRotationEuler(const glm::vec3& euler);
		private:
			glm::vec3 RotationEuler = { 0.0f, 0.0f, 0.0f };
			glm::quat Rotation = { 1.0f, 0.0f, 0.0f, 0.0f };
		};

		struct Mesh
		{
			Mesh() = default;
			Mesh(const Mesh&) = default;
			Mesh(AssetHandle assetHandle);

			AssetHandle handle;
		};

		struct Script
		{
			Script() = default;
			Script(const Script&) = default;
			Script(std::string Name)
				:name(Name)
			{}
			
			std::string name;
		};

		struct RigidBody
		{
			RigidBody() = default;
			RigidBody(const RigidBody&) = default;

			enum class MotionType { Static = 0, Dynamic = 1, Kinematic = 2 };
			MotionType motionType = MotionType::Static;
			float mass = 1.f;

			bool Simulate = true;
		};

		struct BoxCollider
		{
			BoxCollider() = default;
			BoxCollider(const BoxCollider&) = default;

			glm::vec3 HalfExtent = glm::vec3(0.5f);
		};
	}
}