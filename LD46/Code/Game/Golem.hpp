#pragma once
#include "Engine/Renderer/SkeletalMesh.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Time/Timer.hpp"

class GPUMesh;
class Texture;
class Shader;

class Golem
{
public:
	Golem();
	~Golem();

	void Update( float deltaSeconds, Transform chestTransform );
	void Render();

	Mat44 GetHeadMatrix() const;
	Transform GetHeadTransform() const;
	Transform GetChestTransform() const;
	void SetupMaterials( SkeletalMeshBone* boneToAddMaterials, Texture* diffuseTex, Texture* normalTex, Shader* shader );
	void CreateMeshes();
	void CreateSkeleton();
public:
	SkeletalMesh* m_golemMesh = nullptr;
	GPUMesh* m_headMesh = nullptr;
	GPUMesh* m_chestMesh = nullptr;
	GPUMesh* m_shoulderMesh = nullptr;
	GPUMesh* m_hipMesh = nullptr;
	GPUMesh* m_armMesh = nullptr;
	GPUMesh* m_legMesh = nullptr;

	Timer m_jumpTimer;

private:
	SkeletalMeshBone* m_leftShoulder = nullptr;
	SkeletalMeshBone* m_rightShoulder = nullptr;
	SkeletalMeshBone* m_leftElbow = nullptr;
	SkeletalMeshBone* m_rightElbow = nullptr;
	SkeletalMeshBone* m_leftHip = nullptr;
	SkeletalMeshBone* m_rightHip = nullptr;
	SkeletalMeshBone* m_leftKnee = nullptr;
	SkeletalMeshBone* m_rightKnee = nullptr;

	float currentDistanceTraveled = 0.f;
};