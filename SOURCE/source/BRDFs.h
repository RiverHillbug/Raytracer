#pragma once
#include <cassert>
#include "Math.h"

namespace dae
{
	namespace BRDF
	{
		/**
		 * \param kd Diffuse Reflection Coefficient
		 * \param cd Diffuse Color
		 * \return Lambert Diffuse Color
		 */
		static ColorRGB Lambert(float kd, const ColorRGB& cd)
		{
			return (kd * cd) / PI;
		}

		static ColorRGB Lambert(const ColorRGB& kd, const ColorRGB& cd)
		{
			return (kd * cd) / PI;
		}

		/**
		 * \brief todo
		 * \param ks Specular Reflection Coefficient
		 * \param exp Phong Exponent
		 * \param l Incoming (incident) Light Direction
		 * \param v View Direction
		 * \param n Normal of the Surface
		 * \return Phong Specular Color
		 */
		static ColorRGB Phong(float ks, float exp, const Vector3& l, const Vector3& v, const Vector3& n)
		{
			const Vector3 reflection{ l - 2 * Vector3::Dot(n, l) * n };
			const float phongSpecularReflection{ ks * pow(std::max(Vector3::Dot(reflection, v), 0.0f), exp) };

			return ColorRGB(phongSpecularReflection, phongSpecularReflection, phongSpecularReflection);
		}

		/**
		 * \brief BRDF Fresnel Function >> Schlick
		 * \param h Normalized Halfvector between View and Light directions
		 * \param v Normalized View direction
		 * \param f0 Base reflectivity of a surface based on IOR (Indices Of Refrection), this is different for Dielectrics (Non-Metal) and Conductors (Metal)
		 * \return
		 */
		static ColorRGB FresnelFunction_Schlick(const Vector3& h, const Vector3& v, const ColorRGB& f0)
		{
			const float intensity{ f0.r + ((1.0f - f0.r) * pow(1.0f - Vector3::Dot(v, h), 5.0f))};
			return ColorRGB(intensity, intensity, intensity);
		}

		/**
		 * \brief BRDF NormalDistribution >> Trowbridge-Reitz GGX (UE4 implemetation - squared(roughness))
		 * \param n Surface normal
		 * \param h Normalized half vector
		 * \param roughness Roughness of the material
		 * \return BRDF Normal Distribution Term using Trowbridge-Reitz GGX
		 */
		static float NormalDistribution_GGX(const Vector3& n, const Vector3& h, float roughness)
		{
			const float roughnessPow4{ roughness * roughness * roughness * roughness };
			const float denominator{ ((Vector3::Dot(n, h) * Vector3::Dot(n, h)) * (roughnessPow4 - 1.0f) + 1) };
			return roughnessPow4 / (PI * (denominator * denominator));
		}

		/**
		 * \brief BRDF Geometry Function >> Schlick GGX (Direct Lighting + UE4 implementation - squared(roughness))
		 * \param n Normal of the surface
		 * \param v Normalized view direction
		 * \param roughness Roughness of the material
		 * \return BRDF Geometry Term using SchlickGGX
		 */
		static float GeometryFunction_SchlickGGX(const Vector3& n, const Vector3& v, float roughness)
		{
			const float nDotv{ Vector3::Dot(n, v) };
			if (nDotv == 0.0f)
				return 0.0f;

			return nDotv / (nDotv * (1.0f - roughness) + roughness);
		}

		/**
		 * \brief BRDF Geometry Function >> Smith (Direct Lighting)
		 * \param n Normal of the surface
		 * \param v Normalized view direction
		 * \param l Normalized light direction
		 * \param roughness Roughness of the material
		 * \return BRDF Geometry Term using Smith (> SchlickGGX(n,v,roughness) * SchlickGGX(n,l,roughness))
		 */
		static float GeometryFunction_Smith(const Vector3& n, const Vector3& v, const Vector3& l, float roughness)
		{
			const float a{ (roughness * roughness) };
			const float k{ ((a + 1) * (a + 1)) / 8.0f };

			return GeometryFunction_SchlickGGX(n, v, k) * GeometryFunction_SchlickGGX(n, l, k);
		}

	}
}