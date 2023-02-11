#pragma once
#include "Application/Layer.h"

class SandboxLayer : public Nexus::Layer
{
public:
	void OnAttach() override;
	void OnUpdate() override;
	void OnRender() override;
	void OnDetach() override;
	void OnWindowResize(uint32_t width,uint32_t height) override;
};

