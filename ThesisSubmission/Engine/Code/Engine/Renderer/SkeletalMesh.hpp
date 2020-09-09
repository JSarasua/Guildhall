#pragma once

class SkeletalMeshBone;


class SkeletalMesh
{
public:
	SkeletalMesh();
	~SkeletalMesh();

	void Render();

public:
	SkeletalMeshBone* m_rootBone = nullptr; //Everything is relative to the root
	SkeletalMeshBone* m_headBone = nullptr; //What cameras would rotate around;
};