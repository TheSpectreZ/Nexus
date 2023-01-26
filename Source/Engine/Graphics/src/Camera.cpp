#include "Graphics/Camera.h"
#include "glm/gtc/matrix_transform.hpp"

void Nexus::Graphics::CameraController::SetProjection(float width, float height, float fov, float near, float far)
{
	camera->projection = glm::perspective(fov, width / height, near, far);
	camera->projection[1][1] *= -1;
}

void Nexus::Graphics::CameraController::Rotate(float yaw, float pitch)
{
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = -sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);
}

void Nexus::Graphics::CameraController::MoveForward(float dt)
{
	position += front * dt;
}

void Nexus::Graphics::CameraController::MoveBackward(float dt)
{
	position -= front * dt;
}

void Nexus::Graphics::CameraController::MoveRight(float dt)
{
	position += glm::normalize(glm::cross(front, up)) * dt;
}

void Nexus::Graphics::CameraController::MoveLeft(float dt)
{
	position -= glm::normalize(glm::cross(front, up)) * dt;
}

void Nexus::Graphics::CameraController::MoveUp(float dt)
{
	position += up * dt;
}

void Nexus::Graphics::CameraController::MoveDown(float dt)
{
	position -= up * dt;
}

void Nexus::Graphics::CameraController::SetView()
{
	camera->view = glm::lookAt(position, position + front, up);
}