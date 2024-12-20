#include "Scene.h"
#include "Utils.h"
#include "Material.h"
#include "Sphere.h"

namespace dae {

#pragma region Base Scene
	//Initialize Scene with Default Solid Color Material (RED)
	Scene::Scene():
		m_Materials({ new Material_SolidColor({1,0,0})})
	{
		m_SphereGeometries.reserve(32);
		m_PlaneGeometries.reserve(32);
		m_TriangleMeshGeometries.reserve(32);
		m_Lights.reserve(32);
	}

	Scene::~Scene()
	{
		for(auto& pMaterial : m_Materials)
		{
			delete pMaterial;
			pMaterial = nullptr;
		}

		m_Materials.clear();
	}

	bool dae::Scene::TryGetClosestHit(const Ray& ray, HitRecord& closestHit) const
	{
		float smallestHitDistance{ FLT_MAX };

		for (const auto& sphere : m_SphereGeometries)
		{
			HitRecord hit{ };

			if (GeometryUtils::HitTest_Sphere(sphere, ray, hit) && hit.cameraToPointDistance <= smallestHitDistance)
			{
				smallestHitDistance = hit.cameraToPointDistance;
				closestHit = hit;
			}
		}

		for (const auto& plane : m_PlaneGeometries)
		{
			HitRecord hit{ };

			if (GeometryUtils::HitTest_Plane(plane, ray, hit) && hit.cameraToPointDistance <= smallestHitDistance)
			{
				smallestHitDistance = hit.cameraToPointDistance;
				closestHit = hit;
			}
		}

		for (const auto& triangleMesh : m_TriangleMeshGeometries)
		{
			HitRecord hit{ };

			if (GeometryUtils::HitTest_TriangleMesh(triangleMesh, ray, hit) && hit.cameraToPointDistance <= smallestHitDistance)
			{
				smallestHitDistance = hit.cameraToPointDistance;
				closestHit = hit;
			}
		}

		return (smallestHitDistance != FLT_MAX);
	}


	void Scene::Update(dae::Timer* pTimer)
	{
		m_Camera.Update(pTimer);
	}

	bool Scene::DoesHit(const Ray& ray) const
	{
		for (const auto& sphere : m_SphereGeometries)
		{
			if (GeometryUtils::HitTest_Sphere(sphere, ray))
			{
				return true;
			}
		}

		for (const auto& plane : m_PlaneGeometries)
		{
			if (GeometryUtils::HitTest_Plane(plane, ray))
			{
				return true;
			}
		}

		for (const auto& triangleMesh : m_TriangleMeshGeometries)
		{
			if (GeometryUtils::HitTest_TriangleMesh(triangleMesh, ray))
			{
				return true;
			}
		}

		return false;
	}

#pragma region Scene Helpers
	Sphere* Scene::AddSphere(const Vector3& origin, float radius, unsigned char materialIndex)
	{
		Sphere s{ origin, Vector3(1.0f, 1.0f, 1.0f), radius, materialIndex };

		m_SphereGeometries.emplace_back(s);
		return &m_SphereGeometries.back();
	}

	Plane* Scene::AddPlane(const Vector3& origin, const Vector3& normal, unsigned char materialIndex)
	{
		Plane p;
		p.origin = origin;
		p.normal = normal;
		p.materialIndex = materialIndex;

		m_PlaneGeometries.emplace_back(p);
		return &m_PlaneGeometries.back();
	}

	TriangleMesh* Scene::AddTriangleMesh(TriangleCullMode cullMode, unsigned char materialIndex)
	{
		TriangleMesh m{};
		m.cullMode = cullMode;
		m.materialIndex = materialIndex;

		m_TriangleMeshGeometries.emplace_back(m);
		return &m_TriangleMeshGeometries.back();
	}

	Light* Scene::AddPointLight(const Vector3& origin, float intensity, const ColorRGB& color)
	{
		Light l;
		l.origin = origin;
		l.intensity = intensity;
		l.color = color;
		l.type = LightType::Point;

		m_Lights.emplace_back(l);
		return &m_Lights.back();
	}

	Light* Scene::AddDirectionalLight(const Vector3& direction, float intensity, const ColorRGB& color)
	{
		Light l;
		l.direction = direction;
		l.intensity = intensity;
		l.color = color;
		l.type = LightType::Directional;

		m_Lights.emplace_back(l);
		return &m_Lights.back();
	}

	unsigned char Scene::AddMaterial(Material* pMaterial)
	{
		m_Materials.push_back(pMaterial);
		return static_cast<unsigned char>(m_Materials.size() - 1);
	}
#pragma endregion
#pragma endregion

#pragma region SCENE W1
	void Scene_W1::Initialize()
	{
				//default: Material id0 >> SolidColor Material (RED)
		constexpr unsigned char matId_Solid_Red = 0;
		const unsigned char matId_Solid_Blue = AddMaterial(new Material_SolidColor{ colors::Blue });

		const unsigned char matId_Solid_Yellow = AddMaterial(new Material_SolidColor{ colors::Yellow });
		const unsigned char matId_Solid_Green = AddMaterial(new Material_SolidColor{ colors::Green });
		const unsigned char matId_Solid_Magenta = AddMaterial(new Material_SolidColor{ colors::Magenta });

		//Spheres
		AddSphere({ -25.f, 0.f, 100.f }, 50.f, matId_Solid_Red);
		AddSphere({ 25.f, 0.f, 100.f }, 50.f, matId_Solid_Blue);

		//Plane
		AddPlane({ -75.f, 0.f, 0.f }, { 1.f, 0.f,0.f }, matId_Solid_Green);
		AddPlane({ 75.f, 0.f, 0.f }, { -1.f, 0.f,0.f }, matId_Solid_Green);
		AddPlane({ 0.f, -75.f, 0.f }, { 0.f, 1.f,0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f, 75.f, 0.f }, { 0.f, -1.f,0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f, 0.f, 125.f }, { 0.f, 0.f,-1.f }, matId_Solid_Magenta);
	}
#pragma endregion

#pragma region SCENE W2
	void Scene_W2::Initialize()
	{
		m_Camera.SetOrigin({ 0.0f, 3.0f, -9.0f });
		m_Camera.SetFOVAngle(45.0f);

		//default: Material id0 >> SolidColor Material (RED)
		constexpr unsigned char matId_Solid_Red = 0;
		const unsigned char matId_Solid_Blue = AddMaterial(new Material_SolidColor{ colors::Blue });

		const unsigned char matId_Solid_Yellow = AddMaterial(new Material_SolidColor{ colors::Yellow });
		const unsigned char matId_Solid_Green = AddMaterial(new Material_SolidColor{ colors::Green });
		const unsigned char matId_Solid_Magenta = AddMaterial(new Material_SolidColor{ colors::Magenta });

		AddSphere({ -1.75f, 1.0f, 0.0f }, 0.75f, matId_Solid_Red);
		AddSphere({ 0.0f, 1.0f, 0.0f }, 0.75f, matId_Solid_Blue);
		AddSphere({ 1.75f, 1.0f, 0.0f }, 0.75f, matId_Solid_Red);
		AddSphere({ -1.75f, 3.0f, 0.0f }, 0.75f, matId_Solid_Red);
		AddSphere({ 0.0f, 3.0f, 0.0f }, 0.75f, matId_Solid_Blue);
		AddSphere({ 1.75f, 3.0f, 0.0f }, 0.75f, matId_Solid_Red);

		AddPlane({ -5.0f, 0.0f, 0.0f }, { 1.f, 0.f,0.f }, matId_Solid_Green);
		AddPlane({ 5.0f, 0.0f, 0.0f }, { -1.f, 0.f,0.f }, matId_Solid_Green);
		AddPlane({ 0.0f, 0.0f, 0.0f }, { 0.f, 1.f,0.f }, matId_Solid_Yellow);	
		AddPlane({ 0.0f, 10.0f, 0.0f }, { 0.f, -1.f,0.f }, matId_Solid_Yellow);
		AddPlane({ 0.0f, 0.0f, 10.0f }, { 0.f, 0.f,-1.f }, matId_Solid_Magenta);

		AddPointLight({ 0.0f, 5.0f, -5.0f }, 70.0f, colors::White);
		AddPointLight(Vector3{ 0.f, 5.f, 5.f }, 50.f, ColorRGB{ 1.f, .61f, .45f }); //Backlight
		AddPointLight(Vector3{ -2.5f, 5.f, -5.f }, 70.f, ColorRGB{ 1.f, .8f, .45f }); //Front Light Left
		AddPointLight(Vector3{ 2.5f, 2.5f, -5.f }, 50.f, ColorRGB{ .34f, .47f, .68f });
	}
#pragma endregion

#pragma region SCENE W3
	void Scene_W3::Initialize()
	{
		sceneName = "Week 3";
		//m_Camera.SetOrigin({ 0.f, 1.f, -5.f });
		m_Camera.SetOrigin({ 0.0f, 3.0f, -9.0f });
		m_Camera.SetFOVAngle(45.0f);

		const auto matCT_GrayRoughMetal = AddMaterial(new Material_CookTorrence({ .972f, .960f, .915f }, 1.f, 1.f));
		const auto matCT_GrayMediumMetal = AddMaterial(new Material_CookTorrence({ .972f, .960f, .915f }, 1.f, .6f));
		const auto matCT_GraySmoothMetal = AddMaterial(new Material_CookTorrence({ .972f, .960f, .915f }, 1.f, .1f));
		const auto matCT_GrayRoughPlastic = AddMaterial(new Material_CookTorrence({ .75f, .75f, .75f }, .0f, 1.f));
		const auto matCT_GrayMediumPlastic = AddMaterial(new Material_CookTorrence({ .75f, .75f, .75f }, .0f, .6f));
		const auto matCT_GraySmoothPlastic = AddMaterial(new Material_CookTorrence({ .75f, .75f, .75f }, .0f, .1f));

		const auto matLambert_GrayBlue = AddMaterial(new Material_Lambert({ .49f, 0.57f, 0.57f }, 1.f));
		const auto matLambert_White = AddMaterial(new Material_Lambert(colors::White, 1.f));

		AddPlane(Vector3{ 0.f, 0.f, 10.f }, Vector3{ 0.f, 0.f, -1.f }, matLambert_GrayBlue); //BACK
		AddPlane(Vector3{ 0.f, 0.f, 0.f }, Vector3{ 0.f, 1.f, 0.f }, matLambert_GrayBlue); //BOTTOM
		AddPlane(Vector3{ 0.f, 10.f, 0.f }, Vector3{ 0.f, -1.f, 0.f }, matLambert_GrayBlue); //TOP
		AddPlane(Vector3{ 5.f, 0.f, 0.f }, Vector3{ -1.f, 0.f, 0.f }, matLambert_GrayBlue); //RIGHT
		AddPlane(Vector3{ -5.f, 0.f, 0.f }, Vector3{ 1.f, 0.f, 0.f }, matLambert_GrayBlue); //LEFT

		AddSphere(Vector3{ -1.75f, 1.f, 0.f }, .75f, matCT_GrayRoughMetal);
		AddSphere(Vector3{ 0.f, 1.f, 0.f }, .75f, matCT_GrayMediumMetal);
		AddSphere(Vector3{ 1.75f, 1.f, 0.f }, .75f, matCT_GraySmoothMetal);
		AddSphere(Vector3{ -1.75f, 3.f, 0.f }, .75f, matCT_GrayRoughPlastic);
		AddSphere(Vector3{ 0.f, 3.f, 0.f }, .75f, matCT_GrayMediumPlastic);
		AddSphere(Vector3{ 1.75f, 3.f, 0.f }, .75f, matCT_GraySmoothPlastic);

		AddPointLight(Vector3{ 0.f, 5.f, 5.f }, 50.f, ColorRGB{ 1.f, .61f, .45f }); //Backlight
		AddPointLight(Vector3{ -2.5f, 5.f, -5.f }, 70.f, ColorRGB{ 1.f, .8f, .45f }); //Front Light Left
		AddPointLight(Vector3{ 2.5f, 2.5f, -5.f }, 50.f, ColorRGB{ .34f, .47f, .68f });
	}
#pragma endregion

#pragma region W4
	void Scene_W4::Initialize()
	{
		m_Camera.SetOrigin(Vector3(0.0f, 3.0f, -9.0f));
		m_Camera.SetFOVAngle(45.0f);

		const auto matCT_GrayRoughMetal = AddMaterial(new Material_CookTorrence({ .972f, .960f, .915f }, 1.f, 1.f));
		const auto matCT_GrayMediumMetal = AddMaterial(new Material_CookTorrence({ .972f, .960f, .915f }, 1.f, .6f));
		const auto matCT_GraySmoothMetal = AddMaterial(new Material_CookTorrence({ .972f, .960f, .915f }, 1.f, .1f));
		const auto matCT_GrayRoughPlastic = AddMaterial(new Material_CookTorrence({ .75f, .75f, .75f }, .0f, 1.f));
		const auto matCT_GrayMediumPlastic = AddMaterial(new Material_CookTorrence({ .75f, .75f, .75f }, .0f, .6f));
		const auto matCT_GraySmoothPlastic = AddMaterial(new Material_CookTorrence({ .75f, .75f, .75f }, .0f, .1f));

		const auto matLambert_GrayBlue = AddMaterial(new Material_Lambert({ .49f, 0.57f, 0.57f }, 1.f));
		const auto matLambert_White = AddMaterial(new Material_Lambert(colors::White, 1.f));

		AddPlane(Vector3{ 0.f, 0.f, 10.f }, Vector3{ 0.f, 0.f, -1.f }, matLambert_GrayBlue); //BACK
		AddPlane(Vector3{ 0.f, 0.f, 0.f }, Vector3{ 0.f, 1.f, 0.f }, matLambert_GrayBlue); //BOTTOM
		AddPlane(Vector3{ 0.f, 10.f, 0.f }, Vector3{ 0.f, -1.f, 0.f }, matLambert_GrayBlue); //TOP
		AddPlane(Vector3{ 5.f, 0.f, 0.f }, Vector3{ -1.f, 0.f, 0.f }, matLambert_GrayBlue); //RIGHT
		AddPlane(Vector3{ -5.f, 0.f, 0.f }, Vector3{ 1.f, 0.f, 0.f }, matLambert_GrayBlue); //LEFT

		AddSphere(Vector3{-1.75f, 1.f, 0.f}, .75f, matCT_GrayRoughMetal);
		AddSphere(Vector3{ 0.f, 1.f, 0.f }, .75f, matCT_GrayMediumMetal);
		AddSphere(Vector3{ 1.75f, 1.f, 0.f }, .75f, matCT_GraySmoothMetal);
		AddSphere(Vector3{ -1.75f, 3.f, 0.f }, .75f, matCT_GrayRoughPlastic);
		AddSphere(Vector3{ 0.f, 3.f, 0.f }, .75f, matCT_GrayMediumPlastic);
		AddSphere(Vector3{ 1.75f, 3.f, 0.f }, .75f, matCT_GraySmoothPlastic);

		//CW Winding Order!
		const Triangle baseTriangle = { Vector3(-0.75f, 1.5f, 0.0f), Vector3(0.75f, 0.0f, 0.0f), Vector3(-0.75f, 0.0f, 0.0f) };

		AddTriangleMesh(TriangleCullMode::BackFaceCulling, matLambert_White);
		m_TriangleMeshGeometries[0].AppendTriangle(baseTriangle, true);
		m_TriangleMeshGeometries[0].CreateTriangles();
		m_TriangleMeshGeometries[0].Translate({ -1.75f, 4.5f, 0.0f });
		m_TriangleMeshGeometries[0].UpdateTransforms();

		AddTriangleMesh(TriangleCullMode::FrontFaceCulling, matLambert_White);
		m_TriangleMeshGeometries[1].AppendTriangle(baseTriangle, true);
		m_TriangleMeshGeometries[1].CreateTriangles();
		m_TriangleMeshGeometries[1].Translate({ 0.0f, 4.5f, 0.0f });
		m_TriangleMeshGeometries[1].UpdateTransforms();

		AddTriangleMesh(TriangleCullMode::NoCulling, matLambert_White);
		m_TriangleMeshGeometries[2].AppendTriangle(baseTriangle, true);
		m_TriangleMeshGeometries[2].CreateTriangles();
		m_TriangleMeshGeometries[2].Translate({ 1.75f, 4.5f, 0.0f });
		m_TriangleMeshGeometries[2].UpdateTransforms();

		/*m_pBunnyMesh = AddTriangleMesh(TriangleCullMode::BackFaceCulling, matLambert_White);
		Utils::ParseOBJ("Resources/lowpoly_bunny.obj", m_pBunnyMesh->positions, m_pBunnyMesh->normals, m_pBunnyMesh->indices);

		m_pBunnyMesh->CreateTriangles();
		m_pBunnyMesh->Scale({ 2.0f, 2.0f, 2.0f });
		m_pBunnyMesh->UpdateTransforms();*/

		AddPointLight(Vector3{ 0.f, 5.f, 5.f }, 50.f, ColorRGB{ 1.f, .61f, .45f }); //Backlight
		AddPointLight(Vector3{ -2.5f, 5.f, -5.f }, 70.f, ColorRGB{ 1.f, .8f, .45f }); //Front Light Left
		AddPointLight(Vector3{ 2.5f, 2.5f, -5.f }, 50.f, ColorRGB{ .34f, .47f, .68f });
	}

	void Scene_W4::Update(dae::Timer* pTimer)
	{
		Scene::Update(pTimer);

		if(m_pBunnyMesh != nullptr)
		{
			m_pBunnyMesh->RotateY(10.0f * PI * pTimer->GetTotal());
			m_pBunnyMesh->UpdateTransforms();
		}

		for (auto& triangleMesh : m_TriangleMeshGeometries)
		{
			triangleMesh.RotateY(10.0f * PI * pTimer->GetTotal());
			triangleMesh.UpdateTransforms();
		}
	}

#pragma endregion
}
