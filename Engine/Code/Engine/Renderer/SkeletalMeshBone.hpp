#pragma once
#include "Engine/Math/Transform.hpp"
#include <vector>

class GPUMesh;


class SkeletalMeshBone
{
public:
	SkeletalMeshBone();
	SkeletalMeshBone( GPUMesh* mesh, SkeletalMeshBone* parentBone, Transform localTransform );
	~SkeletalMeshBone();

	Mat44 GetRelativeModelMatrix() const;

	void Render();

public:
	Transform m_transform;
	GPUMesh* m_mesh = nullptr;
	SkeletalMeshBone* m_parentBone = nullptr;

	std::vector<SkeletalMeshBone*> m_childBones;

	
};