#include "Sphere.h"

dae::Sphere::Sphere(Vector3 center, Vector3 color, float radius, int materialIndex)
	: m_Center{ center }
	, m_Color{ color }
	, m_Radius{ radius }
	, m_MaterialIndex{ materialIndex }
{

}
