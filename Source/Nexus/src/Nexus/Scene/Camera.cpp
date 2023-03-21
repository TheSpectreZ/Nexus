#include "nxpch.h"
#include "Camera.h"

#include "glm/gtc/matrix_transform.hpp"
#include "Core/Application.h"
#include "Core/Input.h"

void Nexus::CameraController::SetKeyBindings(CameraBindings binding, uint16_t key)
{
	m_KeyBinds[(uint32_t)binding] = key;
}

void Nexus::CameraController::AttachCamera(Camera* camera)
{
	m_Camera = camera;
	m_Camera->position = glm::vec3(0.f, 0.f, -4.f);
}

void Nexus::CameraController::SetPerspectiveProjection(float fovy, float width, float height, float zNear, float zFar)
{
	m_Camera->projection = glm::perspective(fovy, width / height, zNear, zFar);
	m_Camera->projection[1][1] *= -1;
}

void Nexus::CameraController::Move()
{
	Timestep dt = Application::Get()->GetTimeStep();

	for (uint32_t i = 0; i < 6; i++)
	{
		if (Input::IsKeyPressed(m_KeyBinds[i]))
		{
			NEXUS_LOG_DEBUG("{0}: {1}", i, dt.GetSeconds());
			m_Camera->position += GetMovementMultiplier((CameraBindings)i) * dt.GetSeconds();
		}
	}

	m_Camera->view = glm::lookAt(m_Camera->position, m_Camera->position + m_front, m_up);
}

glm::vec3 Nexus::CameraController::GetMovementMultiplier(CameraBindings binding)
{
	switch (binding)
	{
	case Nexus::CameraBindings::FRONT:
		return m_front;
	case Nexus::CameraBindings::BACK:
		return -m_front;
	case Nexus::CameraBindings::UP:
		return m_up;
	case Nexus::CameraBindings::DOWN:
		return -m_up;
	case Nexus::CameraBindings::RIGHT:
		return glm::normalize(glm::cross(m_front, m_up));
	case Nexus::CameraBindings::LEFT:
		return -glm::normalize(glm::cross(m_front, m_up));
	default:
		break;
	}
}
