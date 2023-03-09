#pragma once
#include "Nexus.h"

class EditorLayer : public Layer
{
public:
	void OnAttach() override;
	void OnUpdate() override;
	void OnDetach() override;
private:
	Nexus::Ref<Nexus::Pipeline> m_Pipeline;

	Nexus::Viewport m_viewport;
	Nexus::Scissor m_scissor;
};

