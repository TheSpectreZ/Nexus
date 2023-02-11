#include "Player.h"

#include "Platform/Manager.h"
#include "Platform/Input.h"

#include "Graphics/Presenter.h"

using namespace Nexus;

void Player::Create()
{
	PlayerSpeed = 20.f;

	auto extent = Graphics::Presenter::GetImageExtent();

	AttachCamera(&m_RawCameraData);
	SetPosition({ 0.f,0.f,2.f });
	SetProjection((float)extent.width, (float)extent.height, 45.f, 0.1f, 1000.f);
	Rotate(-90.f, 0.f);
}

void Player::Update()
{
	Move();
}

void Player::OnCallback()
{
	auto extent = Nexus::Graphics::Presenter::GetImageExtent();
	SetProjection((float)extent.width, (float)extent.height, 45.f, 0.1f, 1000.f);
}

void Player::Move()
{
	float dt = Platform::Manager::GetDeltaTime() * PlayerSpeed;

	if (Platform::Input::IsKeyPressed(Key::W))
		MoveForward(dt);

	if (Platform::Input::IsKeyPressed(Key::S))
		MoveBackward(dt);

	if (Platform::Input::IsKeyPressed(Key::A))
		MoveLeft(dt);

	if (Platform::Input::IsKeyPressed(Key::D))
		MoveRight(dt);

	if (Platform::Input::IsKeyPressed(Key::Q))
		MoveDown(dt);

	if (Platform::Input::IsKeyPressed(Key::E))
		MoveUp(dt);

	static bool first = true;
	static float lastX, lastY, xOff, yOff, yaw = -90.f, pitch;

	auto [x, y] = Platform::Input::GetMouseCursorPosition();

	if (first)
	{
		lastX = x;
		lastY = y;
		first = false;
	}

	xOff = x - lastX;
	yOff = y - lastY;

	lastX = x;
	lastY = y;

	if (Platform::Input::IsMouseButtonPressed(Mouse::Right))
	{
		xOff *= 0.5f;
		yOff *= 0.5f;

		yaw += xOff;
		pitch += yOff;

		if (pitch > 89.f)
			pitch = 89.f;
		if (pitch < -89.f)
			pitch = -89.f;


		Rotate(yaw, pitch);
	}

	SetView();
}
