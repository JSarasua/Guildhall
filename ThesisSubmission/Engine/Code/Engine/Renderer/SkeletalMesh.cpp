#include "Engine/Renderer/SkeletalMesh.hpp"
#include "Engine/Renderer/SkeletalMeshBone.hpp"

SkeletalMesh::SkeletalMesh()
{

}

SkeletalMesh::~SkeletalMesh()
{

}

void SkeletalMesh::Render()
{
	m_rootBone->Render();
}

