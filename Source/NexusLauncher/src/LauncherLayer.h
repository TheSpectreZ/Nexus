#pragma once
#include "Nexus.h"

class LauncherLayer : public Nexus::Layer
{
public:
	void OnAttach() override;
	void OnUpdate(Nexus::Timestep step) override;
	void OnRender() override;
	void OnDetach() override;
	void OnWindowResize(int width, int height) override;
private:
	Nexus::Ref<Nexus::Renderpass> m_Renderpass;
	
	Nexus::FramebufferSpecification m_FramebufferSpecs;
	Nexus::Ref<Nexus::Framebuffer> m_Framebuffer;

	Nexus::ProjectSpecifications m_ProjectSpecs;
	void RenderLauncherWindow();

	void GenerateProject(const Nexus::ProjectSpecifications& specs);
	void LaunchEditor();
};

