#include "NxScene/Camera.h"
#include "NxCore/Input.h"
#include "glm/gtc/matrix_transform.hpp"

void Nexus::CameraController::SetKeyBinding(CameraBinding binding, uint16_t key)
{
	m_KeyBinds[(size_t)binding] = key;
}

void Nexus::CameraController::AttachCamera(Camera* camera)
{
	m_Camera = camera;
}

void Nexus::CameraController::SetPerspectiveProjection(float fovy, float width, float height, float zNear, float zFar)
{
	m_Camera->projection = glm::perspective(fovy, width / height, zNear, zFar);
	m_Camera->projection[1][1] *= -1;
}

void Nexus::CameraController::Update(float dt)
{
	for (uint32_t i = 0; i < 6; i++)
	{
		if (Module::Input::Get()->IsKeyPressed(m_KeyBinds[i]))
		{
			m_Camera->position += GetMovementMultiplier((CameraBinding)i) * dt * m_Speed;
		}
	}

	// Mouse
	{
		float x, y;
		Module::Input::Get()->GetMouseCursorPosition(&x, &y);

		static float lastX = x;
		static float lastY = y;
		
		float xOff = x - lastX;
		float yOff = y - lastY;

		lastX = x;
		lastY = y;

		if (Module::Input::Get()->IsMouseButtonPressed( m_KeyBinds.at( (size_t)CameraBinding::MOUSE_UNLOCK) ) )
		{
			xOff *= 0.5f;
			yOff *= 0.5f;

			static float yaw = -90.f;
			static float pitch = 0.f;

			yaw += xOff;
			pitch += yOff;

			if (pitch > 89.f)
				pitch = 89.f;
			if (pitch < -89.f)
				pitch = -89.f;

			m_front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
			m_front.y = -sin(glm::radians(pitch));
			m_front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
			m_front = glm::normalize(m_front);
		}
	}

	m_Camera->view = glm::lookAt(m_Camera->position, m_Camera->position + m_front, m_up);
}

glm::vec3 Nexus::CameraController::GetMovementMultiplier(CameraBinding binding)
{
	switch (binding)
	{
	case Nexus::CameraBinding::FRONT:
		return m_front;
	case Nexus::CameraBinding::BACK:
		return -m_front;
	case Nexus::CameraBinding::UP:
		return m_up;
	case Nexus::CameraBinding::DOWN:
		return -m_up;
	case Nexus::CameraBinding::RIGHT:
		return glm::normalize(glm::cross(m_front, m_up));
	case Nexus::CameraBinding::LEFT:
		return -glm::normalize(glm::cross(m_front, m_up));
	default:
		return glm::vec3(0.f);
	}
}
