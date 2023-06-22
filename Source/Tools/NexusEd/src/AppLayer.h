#pragma once
#include "NxApplication/Layer.h"
#include "NxScene/Scene.h"
#include "NxScene/Camera.h"

class AppLayer : public Nexus::Layer
{
public:
	void OnAttach() override;
	void OnUpdate(float dt) override;
	void OnRender() override;
	void OnDetach() override;
	void OnWindowResize(int width, int height) override;
	~AppLayer() override = default;
private:
	Nexus::Camera m_EditorCamera;
	Nexus::CameraController m_EditorCameraController;
	Nexus::Ref<Nexus::Scene> m_EditorScene;
};
