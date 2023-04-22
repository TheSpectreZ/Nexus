#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Core/TimeStep.h"
#include "Core/KeyCode.h"

namespace Nexus
{
	struct Camera
	{
		glm::mat4 projection;
		glm::mat4 view;
		
		glm::vec3 position;
	};

	enum class CameraBindings
	{
		FRONT = 0, BACK = 1, UP = 2, DOWN = 3, RIGHT = 4, LEFT = 5
	};

	class CameraController
	{
	public:
		void SetKeyBindings(CameraBindings binding, uint16_t key);

		void AttachCamera(Camera* camera);
		void SetPerspectiveProjection(float fovy, float width, float height, float zNear, float zFar);
		void Move();
	private:
		Camera* m_Camera;

		// Key Binds
		std::array<uint16_t, 6> m_KeyBinds;

		glm::vec3 m_front = { 0.f,0.f,-1.f };
		glm::vec3 m_up = { 0.f,1.f,0.f };
		
		glm::vec3 GetMovementMultiplier(CameraBindings binding);
	};

}
