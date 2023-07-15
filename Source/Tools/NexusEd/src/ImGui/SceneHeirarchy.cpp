#include "SceneHeirarchy.h"
#include "NxApplication/FileDialog.h"
#include "NxScene/Component.h"
#include "NxAsset/Asset.h"
#include "NxRenderEngine/ResourcePool.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "NxCore/Logger.h"

namespace NexusEd::ImGuiUtils
{
	bool DrawVec3Control(const char* label, glm::vec3& vector, float reset , float columnWidth )
{
	bool changed = false;

	ImGui::PushID(label);

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);

	ImGui::Text(label);
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0,0 });

	float lineheight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.f;
	ImVec2 buttonsize = { lineheight + 3.f,lineheight };

	auto& BoldFont = ImGui::GetIO().Fonts->Fonts[0];

	ImGui::PushStyleColor(ImGuiCol_Button, { 0.8f,0.1f,0.1f,1.f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.7f,0.1f,0.1f,1.f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.6f,0.1f,0.1f,1.f });
	ImGui::PushFont(BoldFont);
	if (ImGui::Button("X", buttonsize))
	{
		vector.x = reset;
		changed = true;
	}
	ImGui::PopStyleColor(3);
	ImGui::PopFont();

	ImGui::SameLine();
	if (ImGui::DragFloat("##X", &vector.x, 0.1f))
		changed = true;
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, { 0.1f,0.8f,0.1f,1.f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.1f,0.7f,0.1f,1.f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.1f,0.6f,0.1f,1.f });
	ImGui::PushFont(BoldFont);
	if (ImGui::Button("Y", buttonsize))
	{
		vector.y = reset;
		changed = true;
	}
	ImGui::PopStyleColor(3);
	ImGui::PopFont();

	ImGui::SameLine();
	if (ImGui::DragFloat("##Y", &vector.y, 0.1f))
		changed = true;
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, { 0.1f,0.1f,0.8f,1.f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.1f,0.1f,0.7f,1.f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.1f,0.1f,0.6f,1.f });
	ImGui::PushFont(BoldFont);
	if (ImGui::Button("Z", buttonsize))
	{
		vector.z = reset;
		changed = true;
	}
	ImGui::PopStyleColor(3);
	ImGui::PopFont();

	ImGui::SameLine();
	if (ImGui::DragFloat("##Z", &vector.z, 0.1f))
		changed = true;
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PopStyleVar();

	ImGui::Columns(1);

	ImGui::PopID();

	return changed;
}

	ImVec2 ImVec2Sum(ImVec2 a, ImVec2 b)
	{
		return { a.x + b.x,a.y + b.y };
	}
}

using namespace Nexus;

template <typename T>
static void DisplayAddComponentEntry(const std::string& entryName,Nexus::Entity e)
{
	if (!e.HasComponent<T>())
	{
		if (ImGui::MenuItem(entryName.c_str()))
		{
			e.AddComponent<T>();
			ImGui::CloseCurrentPopup();
		}
	}
}

template<typename T, typename UIFuntion>
static void DrawComponent(const std::string& name, Nexus::Entity e, UIFuntion uiFunction)
{
	const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap
		| ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding;

	if (e.HasComponent<T>())
	{
		auto& component = e.GetComponent<T>();

		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;

		ImGui::Separator();
		bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());

		ImGui::PopStyleVar();

		ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
		if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
		{
			ImGui::OpenPopup("ComponentSettings");
		}

		bool removeComponent = false;
		if (ImGui::BeginPopup("ComponentSettings"))
		{
			if (ImGui::MenuItem("Remove component"))
				removeComponent = true;

			ImGui::EndPopup();
		}

		if (open)
		{
			uiFunction(component);
			ImGui::TreePop();
		}

		if (removeComponent)
			e.RemoveComponent<T>();
	}
}

static void LoadMesh(const AssetFilePath& filepath,Component::Mesh& component)
{
	Meshing::Mesh mesh;
	auto [res, id] = Importer::LoadMesh(filepath, mesh);

	if (res)
	{
		auto Mesh = ResourcePool::Get()->AllocateRenderableMesh(mesh, id);
		component.handle = id;
	}
}

static UUID LoadMaterial(const AssetFilePath& filepath)
{
	std::unordered_map<uint8_t, Meshing::Texture> textures;
	Meshing::Material material;
	auto [res, id] = Importer::LoadMaterial(filepath, material, textures);

	if (!res)
		return UUID(true);

	ResourcePool::Get()->AllocateRenderableMaterial(material, textures, id);
	return id;
}

void NexusEd::SceneHeirarchy::SetContext(Ref<Scene> scene)
{
	m_Scene = scene;
}

void NexusEd::SceneHeirarchy::Render()
{
	ImGui::Begin("Scene Heirarchy");
	m_Scene->RunForAllEntities( [&](entt::entity e)
			{
				DrawEntityNode(e);
			}
	);

	if (m_SelectedEntity == entt::null && ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_MouseButtonRight))
	{
		if (ImGui::MenuItem("Create Entity"))
		{
			m_Scene->CreateEntity();
		}
		ImGui::EndPopup();
	}
	if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
	{
		m_SelectedEntity = entt::null;
	}
	ImGui::End();

	ImGui::Begin("Properties");
	if (m_SelectedEntity != entt::null)
	{
		DrawComponents(m_SelectedEntity);
	}
	ImGui::End();
}

void NexusEd::SceneHeirarchy::DrawEntityNode(entt::entity e)
{
	Entity en = { e,m_Scene.get() };
	auto& TagComponent = en.GetComponent<Component::Tag>();

	ImGuiTreeNodeFlags flags = ((e == m_SelectedEntity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
	flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

	bool opened = ImGui::TreeNodeEx((void*)(uint64_t)e, flags, TagComponent.name.c_str());
	
	if (ImGui::IsItemClicked())
		m_SelectedEntity = e;

	bool Delete = false;
	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::MenuItem("Duplicate Entity"))
			m_Scene->DuplicateEntity({ e,m_Scene.get() });
		
		if (ImGui::MenuItem("Delete Entity"))
			Delete = true;

		ImGui::EndPopup();
	}

	if (opened)
		ImGui::TreePop();

	if (Delete)
	{
		m_Scene->DestroyEntity({ e,m_Scene.get() });

		if (m_SelectedEntity == e)
			m_SelectedEntity = entt::null;
	}
}

void NexusEd::SceneHeirarchy::DrawComponents(entt::entity e)
{
	Entity en(e, m_Scene.get());

	if (en.HasComponent<Component::Tag>())
	{

		auto& Tag = en.GetComponent<Component::Tag>();

		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		strncpy_s(buffer, Tag.name.c_str(), sizeof(buffer));

		if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
		{
			Tag.name = std::string(buffer);
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			DisplayAddComponentEntry<Component::Mesh>("Mesh", en);
			DisplayAddComponentEntry<Component::Script>("Script", en);
			DisplayAddComponentEntry<Component::RigidBody>("RigidBody", en);
			DisplayAddComponentEntry<Component::BoxCollider>("BoxCollider", en);
			DisplayAddComponentEntry<Component::SphereCollider>("SphereCollider", en);
			DisplayAddComponentEntry<Component::CylinderCollider>("CylinderCollider", en);
			DisplayAddComponentEntry<Component::CapsuleCollider>("CapsuleCollider", en);
			DisplayAddComponentEntry<Component::DirectionalLight>("DirectionalLight", en);
			DisplayAddComponentEntry<Component::PointLight>("PointLight", en);
			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();
	}

	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap
			| ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding;

		if (en.HasComponent<Component::Transform>())
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(Component::Transform).hash_code(), treeNodeFlags, "Transform");
			ImGui::PopStyleVar();

			if (open)
			{
				auto& component = en.GetComponent<Component::Transform>();
				glm::vec3 rot = component.GetRotationEuler();

				ImGuiUtils::DrawVec3Control("Translation", component.Translation);
				if (ImGuiUtils::DrawVec3Control("Rotation", rot))
				{
					component.SetRotationEuler(rot);
				}
				ImGuiUtils::DrawVec3Control("Scale", component.Scale, 1.f);
				ImGui::TreePop();
			}
		}
	}

	DrawComponent<Component::Mesh>("Mesh", en, [&](auto& component)
		{
			static std::string name;	

			if (!component.handle)
				name = "No Mesh Assigned";
			else
				name = "Valid Mesh";
			

			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - 125.f);
			ImGui::LabelText("Name", "%s", name.c_str());
			ImGui::PopItemWidth();

			ImGui::SameLine();

			if (ImGui::Button("Set", ImVec2(80.f, 25.f)))
				ImGui::OpenPopup("Set Mesh");

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					LoadMesh(path, component);
				}
				ImGui::EndDragDropTarget();
			}

			if (ImGui::BeginPopup("Set Mesh"))
			{
				//if (ImGui::MenuItem("Cube"))
				//	LoadMesh("Resources/Meshes/Cube.NxMesh", component);
				//
				//if (ImGui::MenuItem("Sphere"))
				//	LoadMesh("Resources/Meshes/Sphere.NxMesh", component);
				//
				//if (ImGui::MenuItem("IcoSphere"))
				//	LoadMesh("Resources/Meshes/IcoSphere.NxMesh", component);
				//
				//if (ImGui::MenuItem("Torus"))
				//	LoadMesh("Resources/Meshes/Torus.NxMesh", component);
				//
				//if (ImGui::MenuItem("Cylinder"))
				//	LoadMesh("Resources/Meshes/Cylinder.NxMesh", component);

				ImGui::EndPopup();
			}

			if (component.handle)
			{
				auto mesh = ResourcePool::Get()->GetRenderableMesh(component.handle);
				auto& Submeshes = mesh->GetSubmeshes();
				for (uint32_t i = 0; i < (uint32_t)Submeshes.size(); i++)
				{
					ImGui::LabelText("Material", "Submesh %i", i);
					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
						{
							const wchar_t* path = (const wchar_t*)payload->Data;
							Submeshes[i].materialIndex = LoadMaterial(path);
						}
						ImGui::EndDragDropTarget();
					}	
					ImGui::SameLine();
					
					ImGui::PushID(i);
					if (ImGui::Button("Reset"))
						Submeshes[i].materialIndex = UINT64_MAX;
					ImGui::PopID();
				}
			}

		});

	DrawComponent<Component::Script>("Script", en, [&](auto& component)
		{
			
		});

	DrawComponent<Component::RigidBody>("Rigid Body", en, [&](auto& component)
		{
			static const char* types[3] = { "Static","Dynamic","Kinematic" };
			static uint32_t typeIndex = 0;

			if (ImGui::BeginCombo("MotionType",types[typeIndex]))
			{
				for (uint32_t i = 0; i < 3; i++)
				{
					if (ImGui::Selectable(types[i]))
					{
						typeIndex = i;
						component.motionType = (Component::RigidBody::MotionType)i;
					}
				}
				ImGui::EndCombo();
			}

			ImGui::DragFloat("Mass", &component.mass, 0.1f, 0.f);
			ImGui::DragFloat("Friction", &component.friction, 0.1f, 0.f);
			ImGui::DragFloat("Restitution", &component.restitution, 0.01f, 0.f, 1.f);
			ImGui::Checkbox("Simulate", &component.Simulate);

		});

	DrawComponent<Component::BoxCollider>("Box Collider", en, [&](auto& component)
		{
			ImGuiUtils::DrawVec3Control("Half-Extent", component.HalfExtent, 0.5f);
		});

	DrawComponent<Component::SphereCollider>("Sphere Collider", en, [&](auto& component)
		{
			ImGui::DragFloat("Radius", &component.Radius);
		});

	DrawComponent<Component::CylinderCollider>("Cylinder Collider", en, [&](auto& component)
		{
			ImGui::DragFloat("Radius", &component.Radius);
			ImGui::DragFloat("Half-Height", &component.HalfHeight);
		});

	DrawComponent<Component::CapsuleCollider>("Capsule Collider", en, [&](auto& component)
		{
			ImGui::DragFloat("Top-Radius", &component.TopRadius);
			ImGui::DragFloat("Bottom-Radius", &component.BottomRadius);
			ImGui::DragFloat("Half-Height", &component.HalfHeight);
		});

	DrawComponent<Component::DirectionalLight>("Directional Light", en, [&](auto& component)
		{
			ImGui::ColorEdit4("Color", glm::value_ptr(component.color), ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_Float);
			ImGuiUtils::DrawVec3Control("Direction", component.direction, 1.f);
		});
	
	DrawComponent<Component::PointLight>("Point Light", en, [&](auto& component)
		{
			ImGui::ColorEdit3("Color", glm::value_ptr(component.color),ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_Float);
			ImGui::DragFloat("Size", &component.size, 0.1f, 0.1f);
			ImGui::DragFloat("Intensity", &component.intensity, 0.1f, 0.1f);
			ImGui::DragFloat("Falloff", &component.falloff, 0.1f, 0.1f);
		});
}
