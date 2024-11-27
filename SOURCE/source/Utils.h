#pragma once
#include <cassert>
#include <fstream>
#include <cmath>
#include "Math.h"
#include "DataTypes.h"
#include "Sphere.h"

namespace dae
{
	namespace GeometryUtils
	{
#pragma region Sphere HitTest
		//SPHERE HIT-TESTS
		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			const Vector3 rayToSphere{ sphere.GetCenter() - ray.origin };
			const float distanceToClosestPoint{ Vector3::Dot(rayToSphere, ray.direction) };
			if (distanceToClosestPoint < 0.0f)
			{
				return false;
			}

			const Vector3 closestPointOnRay{ (ray.origin + (ray.direction * distanceToClosestPoint)) };
			const float pointToCenterDistanceSqrd{ (closestPointOnRay - sphere.GetCenter()).SqrMagnitude() };

			if (pointToCenterDistanceSqrd < (sphere.GetRadius() * sphere.GetRadius()))
			{
				const float bSqrd{ (sphere.GetRadius() * sphere.GetRadius()) - pointToCenterDistanceSqrd };
				const float rayToPointDistance{ distanceToClosestPoint - sqrt(bSqrd) };

				if (rayToPointDistance < ray.max && rayToPointDistance > ray.min)
				{
					if (!ignoreHitRecord)
					{
						hitRecord.didHit = true;
						hitRecord.cameraToPointDistance = rayToPointDistance;
						hitRecord.origin = ray.origin + (rayToPointDistance * ray.direction);
						hitRecord.materialIndex = sphere.GetMaterialIndex();
						hitRecord.normal = (hitRecord.origin - sphere.GetCenter()) / sphere.GetRadius();
					}
					return true;
				}
			}

			return false;
		}

		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Sphere(sphere, ray, temp, true);
		}
#pragma endregion

#pragma region Plane HitTest
		//PLANE HIT-TESTS
		inline bool HitTest_Plane(const Plane& plane, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			const Vector3 planeToRayOrigin{ ray.origin - plane.origin };
			const float distanceFromPlaneToRay{ Vector3::Dot(planeToRayOrigin, plane.normal) }; //perpendicular to plane
			const float rayDotNegNormal{ Vector3::Dot(ray.direction, -plane.normal) };

			if (rayDotNegNormal > 0.0f)
			{
				const float rayToPointDistance{ distanceFromPlaneToRay / rayDotNegNormal };

				if (rayToPointDistance < ray.max && rayToPointDistance > ray.min)
				{
					if (!ignoreHitRecord)
					{
						hitRecord.didHit = true;
						hitRecord.cameraToPointDistance = rayToPointDistance;
						hitRecord.origin = ray.origin + (rayToPointDistance * ray.direction);
						hitRecord.materialIndex = plane.materialIndex;
						hitRecord.normal = plane.normal;
					}
					return true;
				}
			}
			return false;
		}

		inline bool HitTest_Plane(const Plane& plane, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Plane(plane, ray, temp, true);
		}
#pragma endregion
#pragma region Triangle HitTest
		//TRIANGLE HIT-TESTS
		inline bool IsPointInTriangleEdge(const Vector3& vertex1, const Vector3& vertex2, const Vector3& point, const Vector3& normal)
		{
			const Vector3 edge{ vertex2 - vertex1 };
			const Vector3 vertexToPoint{ point - vertex1 };

			return Vector3::Dot(Vector3::Cross(edge, vertexToPoint), normal) >= 0.0f;
		}

		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			const float rayDotNormal{ Vector3::Dot(ray.direction, triangle.normal) };

			if  (AreEqual(rayDotNormal, 0.0f) ||
				(triangle.cullMode == TriangleCullMode::BackFaceCulling && rayDotNormal > 0.0f) ||
				(triangle.cullMode == TriangleCullMode::FrontFaceCulling && rayDotNormal < 0.0f))
				return false;

			const Vector3 rayToTriangle{ triangle.v0 - ray.origin };
			const float cameraToPointDistance = Vector3::Dot(rayToTriangle, triangle.normal) / rayDotNormal;

			if (cameraToPointDistance <= ray.min || cameraToPointDistance >= ray.max)
				return false;

			const Vector3 hitPoint{ ray.origin + (ray.direction * cameraToPointDistance) };

			if (IsPointInTriangleEdge(triangle.v0, triangle.v1, hitPoint, triangle.normal) &&
				IsPointInTriangleEdge(triangle.v1, triangle.v2, hitPoint, triangle.normal) &&
				IsPointInTriangleEdge(triangle.v2, triangle.v0, hitPoint, triangle.normal))
			{
				if (!ignoreHitRecord)
				{
					hitRecord.didHit = true;
					hitRecord.cameraToPointDistance = cameraToPointDistance;
					hitRecord.origin = hitPoint;
					hitRecord.materialIndex = triangle.materialIndex;
					hitRecord.normal = triangle.normal;
				}
				return true;
			}

			return false;
		}

		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Triangle(triangle, ray, temp, true);
		}
#pragma endregion
#pragma region TriangeMesh HitTest
		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			float nearestTriangle{ FLT_MAX };

			for (const auto& triangle : mesh.triangles)
			{
				HitRecord currentHitRecord;
				if (HitTest_Triangle(triangle, ray, currentHitRecord, ignoreHitRecord))
				{
					if (ignoreHitRecord)
						return true;

					if (currentHitRecord.cameraToPointDistance < nearestTriangle)
					{
						nearestTriangle = currentHitRecord.cameraToPointDistance;
						hitRecord = currentHitRecord;
					}
				}
			}

			return !ignoreHitRecord && hitRecord.didHit;
		}

		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_TriangleMesh(mesh, ray, temp, true);
		}
#pragma endregion
	}

	namespace LightUtils
	{
		//Direction from target to light
		inline Vector3 GetDirectionToLight(const Light& light, const Vector3 origin)
		{
			return (origin - light.origin).Normalized();
		}

		inline ColorRGB GetRadiance(const Light& light, const Vector3& target)
		{
			switch (light.type)
			{
			case LightType::Directional:
				return light.color * light.intensity;
			case LightType::Point:
				return light.color * (light.intensity / ((light.origin - target).SqrMagnitude()));
			}

			return ColorRGB(0.0f, 0.0f, 0.0f);
		}

		inline float GetObservedArea(const Light& light, const HitRecord& hitRecord)
		{
			const Vector3 direction{ GetDirectionToLight(light, hitRecord.origin) };
			const float dot{ -Vector3::Dot(direction, hitRecord.normal) };
			return std::fmax(dot, 0.0f);
		}
	}

	namespace Utils
	{
		//Just parses vertices and indices
#pragma warning(push)
#pragma warning(disable : 4505) //Warning unreferenced local function
		static bool ParseOBJ(const std::string& filename, std::vector<Vector3>& positions, std::vector<Vector3>& normals, std::vector<int>& indices)
		{
			std::ifstream file(filename);
			if (!file)
				return false;

			std::string sCommand;
			// start a while iteration ending when the end of file is reached (ios::eof)
			while (!file.eof())
			{
				//read the first word of the string, use the >> operator (istream::operator>>) 
				sCommand = "EOF";
				file >> sCommand;

				if (sCommand == "EOF")
					break;

				//use conditional statements to process the different commands
				if (sCommand == "#")
				{
					// Ignore Comment
				}
				else if (sCommand == "v")
				{
					//Vertex
					float x, y, z;
					file >> x >> y >> z;
					positions.push_back({ x, y, z });
				}
				else if (sCommand == "f")
				{
					float i0, i1, i2;
					file >> i0 >> i1 >> i2;

					indices.push_back((int)i0 - 1);
					indices.push_back((int)i1 - 1);
					indices.push_back((int)i2 - 1);
				}
				//read till end of line and ignore all remaining chars
				file.ignore(1000, '\n');

				if (file.eof())
					break;
			}

			//Precompute normals
			for (uint64_t index = 0; index < indices.size(); index += 3)
			{
				uint32_t i0 = indices[index];
				uint32_t i1 = indices[index + 1];
				uint32_t i2 = indices[index + 2];

				Vector3 edgeV0V1 = positions[i1] - positions[i0];
				Vector3 edgeV0V2 = positions[i2] - positions[i0];
				Vector3 normal = Vector3::Cross(edgeV0V1, edgeV0V2);

				if(isnan(normal.x))
				{
					int k = 0;
				}

				normal.Normalize();
				if (isnan(normal.x))
				{
					int k = 0;
				}

				normals.push_back(normal);
			}

			return true;
		}
#pragma warning(pop)
	}
}