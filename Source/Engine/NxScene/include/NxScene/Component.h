#pragma once
#include <string>
#include "NxCore/UUID.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include <glm/gtx/quaternion.hpp>
#include "glm/gtc/type_ptr.hpp"

#ifdef NEXUS_SCENE_SHARED_BUILD
#define NEXUS_SCENE_API __declspec(dllexport)
#else 
#define NEXUS_SCENE_API __declspec(dllimport)
#endif

namespace Nexus
{
	namespace Component
	{
		struct NEXUS_SCENE_API Tag
		{
			Tag() = default;
			Tag(const Tag&) = default;
			Tag(const std::string& Name);
			
			std::string name;
		};

		struct NEXUS_SCENE_API Identity
		{
			Identity();
			Identity(const Identity&) = default;
			Identity(UUID id)
				:uuid(id)
			{}

			UUID uuid;
		};

		struct NEXUS_SCENE_API Transform
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

		struct NEXUS_SCENE_API Mesh
		{
			Mesh() = default;
			Mesh(const Mesh&) = default;
			Mesh(UUID UUID);

			UUID handle;
		};

		struct NEXUS_SCENE_API Script
		{
			Script() = default;
			Script(const Script&) = default;
			Script(std::string Name)
				:name(Name)
			{}
			
			std::string name;
		};

		struct NEXUS_SCENE_API RigidBody
		{
			RigidBody() = default;
			RigidBody(const RigidBody&) = default;

			enum class MotionType { Static = 0, Dynamic = 1, Kinematic = 2 };
			MotionType motionType = MotionType::Static;
			
			float mass = 1.f;
			float friction = 0.2f;
			float restitution = 0.5f;

			bool Simulate = true;
		};

		struct NEXUS_SCENE_API BoxCollider
		{
			BoxCollider() = default;
			BoxCollider(const BoxCollider&) = default;

			glm::vec3 HalfExtent = glm::vec3(0.5f);
		};

		struct NEXUS_SCENE_API SphereCollider
		{
			SphereCollider() = default;
			SphereCollider(const SphereCollider&) = default;

			float Radius = 0.5f;
		};

		struct NEXUS_SCENE_API CapsuleCollider
		{
			CapsuleCollider() = default;
			CapsuleCollider(const CapsuleCollider&) = default;

			float HalfHeight = 0.5f;
			float TopRadius = 0.5f;
			float BottomRadius = 0.5f;
		};

		struct NEXUS_SCENE_API CylinderCollider
		{
			CylinderCollider() = default;
			CylinderCollider(const CylinderCollider&) = default;

			float HalfHeight = 0.5f;
			float Radius = 0.5f;
		};

		struct NEXUS_SCENE_API DirectionalLight
		{
			DirectionalLight() = default;
			DirectionalLight(const DirectionalLight&) = default;

			glm::vec3 direction = glm::vec3(1.f);
			glm::vec3 color = glm::vec3(1.f);
		};

		struct NEXUS_SCENE_API PointLight
		{
			PointLight() = default;
			PointLight(const PointLight&) = default;

			glm::vec3 color = glm::vec3(1.f);
			float size = 0.1f; float intensity = 1.f;
			float falloff = 1.f;
		};
	}
}