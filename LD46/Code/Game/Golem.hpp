#pragma once
#include "Engine/Renderer/SkeletalMesh.hpp"
#include "Engine/Math/Transform.hpp"

class GPUMesh;

class Golem
{
public:
	Golem();
	~Golem();

	void Update( float deltaSeconds, Transform chestTransform );
	void Render();

	Mat44 GetHeadMatrix() const;

	void CreateMeshes();
	void CreateSkeleton();
public:
	SkeletalMesh* m_golemMesh = nullptr;
	GPUMesh* m_sphereMesh = nullptr;
	GPUMesh* m_cubeMesh = nullptr;
};