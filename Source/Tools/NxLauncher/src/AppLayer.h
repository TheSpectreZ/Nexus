#pragma once
#include "NxApplication/Layer.h"
#include "NxCore/ProjectSpecification.h"

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
	Nexus::ProjectSpecifications m_ProjectSpecs;
	void RenderLauncherWindow();

	void GenerateProject(const Nexus::ProjectSpecifications& specs);
	void LaunchEditor();
};
