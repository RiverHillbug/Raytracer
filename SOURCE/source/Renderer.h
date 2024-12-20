#pragma once

#include <cstdint>

struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	class Scene;

	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer() = default;

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Render(Scene* pScene) const;
		void RenderPixel(const Scene* pScene, const uint32_t pixelIndex, const float FOV, const float aspectRatio, const struct Matrix cameraToWorld, const struct Vector3 cameraOrigin) const;
		bool SaveBufferToImage() const;

		void CycleLightingMode();
		void PrintCurrentLightingMode() const;
		inline void ToggleShadows() { m_ShadowsEnabled = !m_ShadowsEnabled; }

	private:

		enum class LightingMode : int
		{
			ObservedArea,
			Radiance,
			BRDF,
			Combined,
		};

		LightingMode m_CurrentLightingMode{ LightingMode::Combined };
		bool m_ShadowsEnabled{ true };
		 
		SDL_Window* m_pWindow{};

		SDL_Surface* m_pBuffer{};
		uint32_t* m_pBufferPixels{};

		int m_Width{};
		int m_Height{};
	};
}
