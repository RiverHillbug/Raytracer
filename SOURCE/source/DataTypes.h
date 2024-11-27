#pragma once
#include <cassert>

#include "Math.h"
#include "vector"

namespace dae
{
#pragma region GEOMETRY
	struct Plane
	{
		Vector3 origin{};
		Vector3 normal{};

		unsigned char materialIndex{ 0 };
	};

	enum class TriangleCullMode
	{
		FrontFaceCulling,
		BackFaceCulling,
		NoCulling
	};

	struct Triangle
	{
		Triangle() = default;
		Triangle(const Vector3& _v0, const Vector3& _v1, const Vector3& _v2, const Vector3& _normal):
			v0{_v0}, v1{_v1}, v2{_v2}, normal{_normal.Normalized()}{}

		Triangle(const Vector3& _v0, const Vector3& _v1, const Vector3& _v2) :
			v0{ _v0 }, v1{ _v1 }, v2{ _v2 }
		{
			const Vector3 edgeV0V1 = v1 - v0;
			const Vector3 edgeV0V2 = v2 - v0;
			normal = Vector3::Cross(edgeV0V1, edgeV0V2).Normalized();
		}

		Vector3 v0{};
		Vector3 v1{};
		Vector3 v2{};

		Vector3 normal{};

		TriangleCullMode cullMode{};
		unsigned char materialIndex{};
	};

	struct TriangleMesh
	{
		TriangleMesh() = default;
		TriangleMesh(const std::vector<Vector3>& _positions, const std::vector<int>& _indices, TriangleCullMode _cullMode):
		positions(_positions), indices(_indices), cullMode(_cullMode)
		{
			CreateTriangles();
			UpdateTransforms();
		}

		TriangleMesh(const std::vector<Vector3>& _positions, const std::vector<int>& _indices, const std::vector<Vector3>& _normals, TriangleCullMode _cullMode) :
			positions(_positions), indices(_indices), normals(_normals), cullMode(_cullMode)
		{
			CreateTriangles();
			UpdateTransforms();
		}

		std::vector<Triangle> triangles{};
		std::vector<Vector3> positions{};
		std::vector<Vector3> normals{};
		std::vector<int> indices{};
		unsigned char materialIndex{};

		TriangleCullMode cullMode{TriangleCullMode::BackFaceCulling};

		Matrix rotationTransform{};
		Matrix translationTransform{};
		Matrix scaleTransform{};

		std::vector<Vector3> transformedPositions{};
		std::vector<Vector3> transformedNormals{};

		void Translate(const Vector3& translation)
		{
			translationTransform = Matrix::CreateTranslation(translation);
		}

		void RotateY(float yaw)
		{
			rotationTransform = Matrix::CreateRotationY(yaw);
		}

		void Scale(const Vector3& scale)
		{
			scaleTransform = Matrix::CreateScale(scale);
		}

		void AppendTriangle(const Triangle& triangle, bool ignoreTransformUpdate = false)
		{
			int startIndex = static_cast<int>(positions.size());

			positions.push_back(triangle.v0);
			positions.push_back(triangle.v1);
			positions.push_back(triangle.v2);

			indices.push_back(startIndex);
			indices.push_back(++startIndex);
			indices.push_back(++startIndex);

			normals.push_back(triangle.normal);

			//Not ideal, but making sure all vertices are updated
			if(!ignoreTransformUpdate)
				UpdateTransforms();
		}

		void CreateTriangles()
		{
			const int triangleCount{ int(indices.size()) / 3 };

			triangles.clear();

			for (int i{ 0 }; i < triangleCount; ++i)
			{
				const Vector3 v0{ positions[indices[3 * i]] };
				const Vector3 v1{ positions[indices[(3 * i) + 1]] };
				const Vector3 v2{ positions[indices[(3 * i) + 2]] };

				triangles.emplace_back(v0, v1, v2, normals[i]);
				triangles[i].materialIndex = materialIndex;
				triangles[i].cullMode = cullMode;
			}
		}

		void CalculateNormals()
		{
			const int triangleCount{ int(indices.size()) / 3 };
			for (int i{ 0 }; i < triangleCount; ++i)
			{
				const Vector3 v0{ positions[indices[3 * i]] };
				const Vector3 v1{ positions[indices[(3 * i) + 1]] };
				const Vector3 v2{ positions[indices[(3 * i) + 2]] };
				const Vector3 a{ v1 - v0 };
				const Vector3 b{ v2 - v0 };
				const Vector3 normal{ Vector3::Cross(a, b) };

				normals.emplace_back(normal);
			}
		}

		void UpdateTransforms()
		{
			for (int i{ 0 }; i < int(triangles.size()); ++i)
			{
				Vector3 transformedPosition1{ };
				transformedPosition1 = scaleTransform * positions[indices[3 * i]];
				transformedPosition1 = rotationTransform * transformedPosition1;
				transformedPosition1 = translationTransform * transformedPosition1;

				transformedPositions.emplace_back(transformedPosition1);

				Vector3 transformedPosition2{ };
				transformedPosition2 = scaleTransform * positions[indices[(3 * i) + 1]];
				transformedPosition2 = rotationTransform * transformedPosition2;
				transformedPosition2 = translationTransform * transformedPosition2;

				transformedPositions.emplace_back(transformedPosition2);

				Vector3 transformedPosition3{ };
				transformedPosition3 = scaleTransform * positions[indices[(3 * i) + 2]];
				transformedPosition3 = rotationTransform * transformedPosition3;
				transformedPosition3 = translationTransform * transformedPosition3;

				transformedPositions.emplace_back(transformedPosition3);

				triangles[i].v0 = transformedPosition1;
				triangles[i].v1 = transformedPosition2;
				triangles[i].v2 = transformedPosition3;

				const Vector3 transformedNormal{ rotationTransform * normals[i] };
				transformedNormals.emplace_back(transformedNormal);
				triangles[i].normal = transformedNormal;
			}
		}
	};
#pragma endregion
#pragma region LIGHT
	enum class LightType
	{
		Point,
		Directional
	};

	struct Light
	{
		Vector3 origin{};
		Vector3 direction{};
		ColorRGB color{};
		float intensity{};

		LightType type{};
	};
#pragma endregion
#pragma region MISC
	struct Ray
	{
		Vector3 origin{};
		Vector3 direction{};

		float min{ 0.0001f };
		float max{ FLT_MAX };
	};

	struct HitRecord
	{
		Vector3 origin{};
		Vector3 normal{};
		float cameraToPointDistance = FLT_MAX; //t

		bool didHit{ false };
		unsigned char materialIndex{ 0 };
	};
#pragma endregion
}