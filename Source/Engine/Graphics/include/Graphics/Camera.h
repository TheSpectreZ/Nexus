#pragma once
#include "Build.h"
#include "glm/glm.hpp"

namespace Nexus
{
	namespace Graphics
	{
		struct Camera
		{
			glm::mat4 projection, view;
		};

		class CameraController
		{
			glm::vec3 front = { 0.f,0.f,-1.f };
			glm::vec3 up = { 0.f,1.f,0.f };
		public:
			void AttachCamera(Camera* cam) { camera = cam; }
			void SetPosition(glm::vec3 pos) { position = pos; }
			
			void SetProjection(float width, float height, float fov, float near, float far);
			
			void Rotate(float yaw, float pitch);

			void MoveForward(float dt);
			void MoveBackward(float dt);
			void MoveRight(float dt);
			void MoveLeft(float dt);
			void MoveUp(float dt);
			void MoveDown(float dt);

			void Update();
		private:
			Camera* camera;

			glm::vec3 position;
		};
	}
}