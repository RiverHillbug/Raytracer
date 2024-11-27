#pragma once
#include <cassert>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#include "Math.h"
#include "Timer.h"

namespace dae
{
	class Camera
	{
	public:
		Camera();
		Camera(const Vector3& _origin, float _fovAngle);
		void Update(Timer* pTimer);
		Matrix CalculateCameraToWorld();
		inline Vector3 GetOrigin() const { return m_Origin; }
		inline void SetOrigin(Vector3 origin) { m_Origin = origin; }
		inline float GetFOVAngle() const { return m_FOVAngle; }
		inline void SetFOVAngle(float fovAngle) {m_FOVAngle = fovAngle; }
		inline Matrix GetCameraToWorld() const { return m_CameraToWorld; }

	private:
		Vector3 m_Origin{ };
		float m_FOVAngle{70.f};

		Vector3 m_Forward{ Vector3::UnitZ };
		Vector3 m_Up{ Vector3::UnitY };
		Vector3 m_Right{ Vector3::UnitX };

		float m_TotalPitch{ 0.0f };
		float m_TotalYaw{ 0.0f };
		const float m_CameraMovementSpeed{ 3.0f };
		const float m_CameraRotationSpeed{ 2.0f };

		Matrix m_CameraToWorld{ };
	};
}
