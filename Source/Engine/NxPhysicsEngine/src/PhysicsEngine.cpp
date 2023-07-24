#include "NxPhysicsEngine/PhysicsEngine.h"
#include "NxCore/Logger.h"
#include "NxCore/Assertion.h"

#include "NxScene/Entity.h"

#include "PhysX/PxConfig.h"	
#include "PhysX/PxPhysicsAPI.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#define PVD_HOST "127.0.0.1"

glm::vec3 Px_glmVec3(physx::PxVec3 vect)
{
	return { vect.x,vect.y,vect.z };
}

glm::quat Px_glmQuat(physx::PxQuat quat)
{
	return { quat.x,quat.y,quat.z,quat.w };
}

struct PhysicsEngineData
{
	physx::PxDefaultAllocator m_defaultAllocator;
	physx::PxDefaultErrorCallback m_defaultErrorCallback;

	physx::PxFoundation* m_foundation;
	physx::PxPvd* m_pvd;
	physx::PxPhysics* m_physics;

	physx::PxDefaultCpuDispatcher* m_dispatcher;
	physx::PxScene* m_scene;
	physx::PxMaterial* m_defaultMaterial;

	Nexus::Scene* m_currentScenePtr;
	std::unordered_map<uint32_t, physx::PxRigidActor*> m_actors;

	~PhysicsEngineData()
	{
		PX_RELEASE(m_dispatcher);

		PX_RELEASE(m_physics);
		if (m_pvd)
		{
			auto transport = m_pvd->getTransport();
			m_pvd->release(); m_pvd = NULL;
			PX_RELEASE(transport);
		}
		PX_RELEASE(m_foundation);
	}
};

static PhysicsEngineData* s_Data;
Nexus::PhysicsEngine* Nexus::PhysicsEngine::s_Instance = nullptr;

void Nexus::PhysicsEngine::Initialize()
{
	s_Data = new PhysicsEngineData;
	s_Instance = new PhysicsEngine;

	s_Data->m_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, s_Data->m_defaultAllocator, s_Data->m_defaultErrorCallback);
	NEXUS_BREAK_ASSERT((!s_Data->m_foundation), "PhysX Create Foundation Failed");

	s_Data->m_pvd = physx::PxCreatePvd(*s_Data->m_foundation);
	auto transport = physx::PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	s_Data->m_pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

	s_Data->m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *s_Data->m_foundation, physx::PxTolerancesScale(), true, nullptr);
	NEXUS_BREAK_ASSERT((!s_Data->m_physics), "PhysX Create Physics object Failed");

	s_Data->m_dispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	s_Data->m_defaultMaterial = s_Data->m_physics->createMaterial(0.5f, 0.5f, 0.6f);	
}

void Nexus::PhysicsEngine::Shutdown()
{
	delete s_Instance;
	delete s_Data;
}

void Nexus::PhysicsEngine::OnSceneStart(Ref<Scene> scene)
{
	physx::PxSceneDesc desc(s_Data->m_physics->getTolerancesScale());
	desc.gravity = physx::PxVec3(0.f, -9.81f, 0.f);
	desc.cpuDispatcher = s_Data->m_dispatcher;
	desc.filterShader = physx::PxDefaultSimulationFilterShader;

	s_Data->m_scene = s_Data->m_physics->createScene(desc);
	s_Data->m_currentScenePtr = scene.get();

	Entity entity;
	auto view = scene->GetAllEntitiesWith<Component::RigidBody>();
	for (auto e : view)
	{
		entity = { e,scene.get() };
		s_Instance->CreateRigidbody(entity);
	}
}

void Nexus::PhysicsEngine::OnSceneUpdate(float dt)
{
	s_Data->m_scene->simulate(dt);
	s_Data->m_scene->fetchResults(true);

	// Update
	{
		Entity entity;
		for (auto& [e, a] : s_Data->m_actors)
		{
			entity = { entt::entity(e),s_Data->m_currentScenePtr };
			physx::PxTransform pxTransform = a->getGlobalPose();
			
			auto& tc = entity.GetComponent<Component::Transform>();
			tc.Translation = Px_glmVec3(pxTransform.p);
			tc.SetRotation(Px_glmQuat(pxTransform.q));
		}
	}
}

void Nexus::PhysicsEngine::OnSceneStop()
{
	s_Data->m_scene->release();
}

void Nexus::PhysicsEngine::CreateRigidbody(Entity e)
{
	if (e.HasComponent<Component::PlaneCollider>())
	{
		auto& tc = e.GetComponent<Component::Transform>();

		auto normal = glm::vec3(glm::toMat3(tc.GetRotation())[2]);
		auto position = glm::length(tc.Translation);

		physx::PxRigidStatic* plane = physx::PxCreatePlane(*s_Data->m_physics,
			physx::PxPlane(normal.x, normal.y, normal.z, position), 
			*s_Data->m_defaultMaterial);
		
		s_Data->m_scene->addActor(*plane);
		//s_Data->m_actors[(uint32_t)e] = plane;

		return;
	}

	if (e.HasComponent<Component::BoxCollider>())
	{
		auto& bc = e.GetComponent<Component::BoxCollider>();

		auto geometry = physx::PxBoxGeometry(bc.HalfExtent.x, bc.HalfExtent.y, bc.HalfExtent.z);
		physx::PxShape* shape = s_Data->m_physics->createShape(geometry, *s_Data->m_defaultMaterial);
	
		auto& tc = e.GetComponent<Component::Transform>();
		
		physx::PxTransform transform = physx::PxTransform(physx::PxVec3(tc.Translation.x, tc.Translation.y, tc.Translation.z));

		auto& rb = e.GetComponent<Component::RigidBody>();
		if (rb.motionType == Component::RigidBody::MotionType::Dynamic)
		{
			physx::PxRigidDynamic* body = s_Data->m_physics->createRigidDynamic(transform);
			body->attachShape(*shape);

			physx::PxRigidBodyExt::updateMassAndInertia(*body, 10.f);
			s_Data->m_scene->addActor(*body);
			s_Data->m_actors[(uint32_t)e] = body;
		}
		else
		{
			physx::PxRigidStatic* body = s_Data->m_physics->createRigidStatic(transform);
			body->attachShape(*shape);
			s_Data->m_scene->addActor(*body);
		}

		return;
	}
}
