
#include "Camera.h"
#include <iostream>

namespace dae
{
	Camera::Camera(const Vector3& origin, float fovAngle)
		: m_Origin{ origin }
		, m_FOVAngle{ fovAngle }
	{
		CalculateCameraToWorld();
	}

	Camera::Camera()
		: m_Origin{ }
		, m_FOVAngle{ }
	{
		CalculateCameraToWorld();
	}

	Matrix Camera::CalculateCameraToWorld()
	{
		m_CameraToWorld = Matrix::CreateRotation(m_TotalPitch, m_TotalYaw, 0.0f);
		m_CameraToWorld *= Matrix::CreateTranslation(m_Origin);
		return m_CameraToWorld;
	}

	void Camera::Update(Timer* pTimer)
	{
		const float deltaTime = pTimer->GetElapsed();

		//Keyboard Input
		const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);

		Vector3 movementDirection{ };

		if (pKeyboardState[SDL_SCANCODE_W])
		{
			++movementDirection.z;
		}
		if (pKeyboardState[SDL_SCANCODE_A])
		{
			--movementDirection.x;
		}
		if (pKeyboardState[SDL_SCANCODE_S])
		{
			--movementDirection.z;
		}
		if (pKeyboardState[SDL_SCANCODE_D])
		{
			++movementDirection.x;
		}

		//Mouse Input
		int mouseX{}, mouseY{};
		const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

		if (mouseState & SDL_BUTTON_RMASK && mouseState & SDL_BUTTON_LMASK)
		{
			movementDirection.y -= mouseY;
		}
		else if (mouseState & SDL_BUTTON_RMASK)
		{
			m_TotalYaw += mouseX * m_CameraRotationSpeed * deltaTime;
			m_TotalPitch -= mouseY * m_CameraRotationSpeed * deltaTime;
		}
		else if (mouseState & SDL_BUTTON_LMASK)
		{
			m_TotalYaw += mouseX * m_CameraRotationSpeed * deltaTime;
			movementDirection.z -= mouseY;
		}

		if (movementDirection != Vector3::Zero)
		{
			movementDirection = (movementDirection.x * m_CameraToWorld.GetAxisX()) + (movementDirection.y * m_CameraToWorld.GetAxisY()) + (movementDirection.z * m_CameraToWorld.GetAxisZ());

			m_Origin += movementDirection.Normalized() * m_CameraMovementSpeed * deltaTime;
		}
		CalculateCameraToWorld();
	}
}
