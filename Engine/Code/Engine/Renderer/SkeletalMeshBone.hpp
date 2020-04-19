#pragma once
#include "Engine/Math/Transform.hpp"
#include <vector>

class GPUMesh;
class Texture;
class Shader;


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
	Texture* m_diffuseTex = nullptr;
	Texture* m_normalTex = nullptr;
	Shader* m_shader = nullptr;
	SkeletalMeshBone* m_parentBone = nullptr;

	std::vector<SkeletalMeshBone*> m_childBones;

	
};