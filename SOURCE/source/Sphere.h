#pragma once
#include "Vector3.h"
namespace dae
{
	class Sphere
	{
	public:
		Sphere(Vector3 center, Vector3 color, float radius, int materialIndex);
		inline Vector3 GetCenter() const { return m_Center; }
		inline Vector3 GetColor() const { return m_Color; }
		inline float GetRadius() const { return m_Radius; }
		inline int GetMaterialIndex() const { return m_MaterialIndex; }
	private:
		Vector3 m_Center;
		Vector3 m_Color;
		float m_Radius;
		int m_MaterialIndex;
	};

}
