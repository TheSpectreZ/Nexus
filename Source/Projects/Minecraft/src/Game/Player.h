#pragma once
#include "Graphics/Camera.h"

class Player : public Nexus::Graphics::CameraController
{
public:
	void Create();
	void Update();
	void OnCallback();
public:
	float PlayerSpeed;

	Nexus::Graphics::Camera& GetRawCameraData() { return m_RawCameraData; }
private:
	Nexus::Graphics::Camera m_RawCameraData;
	
	void Move();
};

