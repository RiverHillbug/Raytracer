//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "Renderer.h"
#include "Math.h"
#include "Matrix.h"
#include "Material.h"
#include "Scene.h"
#include "Utils.h"
#include "Sphere.h"
#include <iostream>
#include <execution>

#define PARALLEL_EXECUTION

using namespace dae;

Renderer::Renderer(SDL_Window * pWindow) :
	m_pWindow(pWindow),
	m_pBuffer(SDL_GetWindowSurface(pWindow))
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);
	m_pBufferPixels = static_cast<uint32_t*>(m_pBuffer->pixels);
}

void Renderer::Render(Scene* pScene) const
{
	Camera& camera = pScene->GetCamera();
	auto& materials = pScene->GetMaterials();
	auto& lights = pScene->GetLights();

	const Vector3 right{ camera.GetCameraToWorld().GetAxisX() };
	const Vector3 up{ camera.GetCameraToWorld().GetAxisY() };
	const Vector3 forward{ camera.GetCameraToWorld().GetAxisZ() };

	const float aspectRatio{ float(m_Width) / float(m_Height) };
	const float FOV{ tan((dae::TO_RADIANS * camera.GetFOVAngle()) / 2.0f) };

	const uint32_t pixelCount{ uint32_t(m_Width * m_Height) };

#ifdef PARALLEL_EXECUTION

	std::vector<uint32_t> pixelIndices{ };

	pixelIndices.reserve(pixelCount);

	for (uint32_t pixel{ 0 }; pixel < pixelCount; ++pixel)
	{
		pixelIndices.emplace_back(pixel);
	}

	std::for_each(std::execution::par, pixelIndices.begin(), pixelIndices.end(),
		[&](int i){ RenderPixel(pScene, i, FOV, aspectRatio, camera.GetCameraToWorld(), camera.GetOrigin()); });	

#else

	for (uint32_t pixel{ 0 }; pixel < pixelCount; ++pixel)
	{
		RenderPixel(pScene, pixel, FOV, aspectRatio, camera.GetCameraToWorld(), camera.GetOrigin());
	}

#endif

	//@END
	//Update SDL Surface
	SDL_UpdateWindowSurface(m_pWindow);
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBuffer, "RayTracing_Buffer.bmp");
}

void Renderer::CycleLightingMode()
{
	switch (m_CurrentLightingMode)
	{
	case dae::Renderer::LightingMode::ObservedArea:
		m_CurrentLightingMode = LightingMode::Radiance;
		break;

	case dae::Renderer::LightingMode::Radiance:
		m_CurrentLightingMode = Renderer::LightingMode::BRDF;
		break;

	case dae::Renderer::LightingMode::BRDF:
		m_CurrentLightingMode = Renderer::LightingMode::Combined;
		break;

	case dae::Renderer::LightingMode::Combined:
		m_CurrentLightingMode = Renderer::LightingMode::ObservedArea;
		break;
	}

	PrintCurrentLightingMode();
}

void Renderer::PrintCurrentLightingMode() const
{
	switch (m_CurrentLightingMode)
	{
	case dae::Renderer::LightingMode::ObservedArea:
		std::cout << "\nLighting Mode: Observed Area\n";
		break;

	case dae::Renderer::LightingMode::Radiance:
		std::cout << "\nLighting Mode: Radiance\n";
		break;

	case dae::Renderer::LightingMode::BRDF:
		std::cout << "\nLighting Mode: BDRF\n";
		break;

	case dae::Renderer::LightingMode::Combined:
		std::cout << "\nLighting Mode: Combined\n";
		break;
	}
}

void Renderer::RenderPixel(const Scene* pScene, const uint32_t pixelIndex, const float FOV, const float aspectRatio, const Matrix cameraToWorld, const Vector3 cameraOrigin) const
{
	auto materials{ pScene->GetMaterials() };
	const uint32_t px{ pixelIndex % m_Width };
	const uint32_t py{ pixelIndex / m_Width };

	const float cX{ (((2.0f * (px + 0.5f)) / m_Width) - 1.0f) * aspectRatio * FOV };
	const float cY{ (1.0f - ((2.0f * py) / m_Height)) * FOV };

	Vector3 rayDirection{ (cX * cameraToWorld.GetAxisX()) + (cY * cameraToWorld.GetAxisY()) + cameraToWorld.GetAxisZ() };
	rayDirection = rayDirection.Normalized();

	Ray hitRay{ cameraOrigin, rayDirection };

	ColorRGB finalColor{ 0.0f, 0.0f, 0.0f };
	HitRecord hitRecord{ };

	if (pScene->TryGetClosestHit(hitRay, hitRecord))
	{
		for (const auto& light : pScene->GetLights())
		{
			bool isInShadow{ false };

			const Vector3 hitToLight{ light.origin - hitRecord.origin };
			const float distanceFromLight{ hitToLight.Magnitude() };
			const Vector3 directionToLight{ hitToLight / distanceFromLight };

			if (m_ShadowsEnabled)
			{
				Ray pointToLight{ hitRecord.origin, directionToLight };
				pointToLight.max = distanceFromLight;
				pointToLight.min = 0.01f;

				if (pScene->DoesHit(pointToLight))
				{
					isInShadow = true;
				}
			}

			if (!isInShadow)
			{
				const float illumination{ LightUtils::GetObservedArea(light, hitRecord) };
				const ColorRGB radiance = LightUtils::GetRadiance(light, hitRecord.origin);
				const ColorRGB brdf = materials[hitRecord.materialIndex]->Shade(hitRecord, directionToLight, -rayDirection);

				if (m_CurrentLightingMode == Renderer::LightingMode::Radiance)
				{
					finalColor += radiance;
				}
				else if (m_CurrentLightingMode == Renderer::LightingMode::ObservedArea)
				{
					finalColor += ColorRGB(illumination, illumination, illumination);
				}
				else if (m_CurrentLightingMode == Renderer::LightingMode::BRDF)
				{
					finalColor += brdf;
				}
				else if (m_CurrentLightingMode == LightingMode::Combined)
				{
					finalColor += radiance * brdf * illumination;
				}
			}
		}
	}

	//Update Color in Buffer
	finalColor.MaxToOne();

	m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
		static_cast<uint8_t>(finalColor.r * 255),
		static_cast<uint8_t>(finalColor.g * 255),
		static_cast<uint8_t>(finalColor.b * 255));
}
