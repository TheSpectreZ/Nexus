#include "NxPhysicsEngine/PhysicsEngine.h"
#include "NxCore/Logger.h"
#include "NxCore/Assertion.h"

#include "NxScene/Entity.h"

#include "PhysX/PxConfig.h"	
#include "PhysX/PxPhysicsAPI.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "NxRenderEngine/BatchRenderer.h"

#define PVD_HOST "127.0.0.1"

template<typename A,typename B> 
A ConvertVec3(const B& b)
{
	return { b.x,b.y,b.z };
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
	s_Data->m_scene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.f);
	s_Data->m_scene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, 2.f);
//s_Data->m_currentScenePtr = scene.get();

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

			uint32_t size = a->getNbShapes();
			std::vector<physx::PxShape*> shapes(size);
			a->getShapes(shapes.data(), size);

			physx::PxTransform pxTransform = a->getGlobalPose();
			
			if (!shapes.empty())
			{
				for(auto sh : shapes)
					pxTransform *= sh->getLocalPose();
			}
			
			auto& tc = entity.GetComponent<Component::Transform>();
			tc.Translation = ConvertVec3<glm::vec3>(pxTransform.p);
			tc.SetRotation(glm::quat(pxTransform.q.w, pxTransform.q.x, pxTransform.q.y, pxTransform.q.z));
		}
	}
}

void Nexus::PhysicsEngine::OnSceneStop()
{
	s_Data->m_scene->release();
	s_Data->m_currentScenePtr = nullptr;
	s_Data->m_actors.clear();
}

void Nexus::PhysicsEngine::DrawColliders()
{
	auto& rb = s_Data->m_scene->getRenderBuffer();

	for (uint32_t i = 0; i < rb.getNbLines(); i++)
	{
		auto& line = rb.getLines()[i];

		BatchRenderer::Get()->DrawLine(
			ConvertVec3<glm::vec3>(line.pos0),
			ConvertVec3<glm::vec3>(line.pos1)
		);
	}

	//for (uint32_t i = 0; i < rb.getNbTriangles(); i++)
	//{
	//	auto& tri = rb.getTriangles()[i];
	//	
	//	BatchRenderer::Get()->DrawTriangle(
	//		ConvertVec3<glm::vec3>(tri.pos0),
	//		ConvertVec3<glm::vec3>(tri.pos1),
	//		ConvertVec3<glm::vec3>(tri.pos2)
	//	);
	//}
}

void Nexus::PhysicsEngine::CreateRigidbody(Entity e)
{
	auto& tc = e.GetComponent<Component::Transform>();
	auto quat = tc.GetRotation();
	physx::PxTransform transform = physx::PxTransform(ConvertVec3<physx::PxVec3>(tc.Translation), physx::PxQuat(quat.x, quat.y, quat.z, quat.w));

	if (e.HasComponent<Component::PlaneCollider>())
	{
		physx::PxPlaneGeometry geometry;
		physx::PxShape* shape = s_Data->m_physics->createShape(geometry, *s_Data->m_defaultMaterial);

		physx::PxRigidStatic* actor = s_Data->m_physics->createRigidStatic(transform);
		actor->attachShape(*shape);
		s_Data->m_scene->addActor(*actor);
		return;
	}

	physx::PxRigidActor* actor = nullptr;
	physx::PxMaterial* material = nullptr;

	auto& rb = e.GetComponent<Component::RigidBody>();
	material = s_Data->m_physics->createMaterial(rb.friction, rb.friction, rb.restitution);

	switch (rb.motionType)
	{
	case Component::RigidBody::MotionType::Static:
		actor = s_Data->m_physics->createRigidStatic(transform);
		break;
	case Component::RigidBody::MotionType::Dynamic:
		actor = s_Data->m_physics->createRigidDynamic(transform);
		physx::PxRigidBodyExt::updateMassAndInertia((physx::PxRigidBody&)*actor, 10.f);
		s_Data->m_actors[(uint32_t)e] = actor;
		break;
	default:
		return;
	}
	s_Data->m_scene->addActor(*actor);

	if (e.HasComponent<Component::BoxCollider>())
	{
		auto& bc = e.GetComponent<Component::BoxCollider>();
		auto geometry = physx::PxBoxGeometry(bc.HalfExtent.x, bc.HalfExtent.y, bc.HalfExtent.z);
		
		physx::PxShape* shape = s_Data->m_physics->createShape(geometry, *material);
		actor->attachShape(*shape);
	}

	if (e.HasComponent<Component::SphereCollider>())
	{
		auto& sc = e.GetComponent<Component::SphereCollider>();
		auto geometry = physx::PxSphereGeometry(sc.Radius);

		physx::PxShape* shape = s_Data->m_physics->createShape(geometry, *material);
		actor->attachShape(*shape);
	}

	if (e.HasComponent<Component::CapsuleCollider>())
	{
		auto& cc = e.GetComponent<Component::CapsuleCollider>();
		auto geometry = physx::PxCapsuleGeometry(cc.Radius, cc.HalfHeight);

		physx::PxShape* shape = s_Data->m_physics->createShape(geometry, *material);
		actor->attachShape(*shape);
	}
}
