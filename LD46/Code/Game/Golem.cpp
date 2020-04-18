#include "Game/Golem.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/SkeletalMeshBone.hpp"

extern RenderContext* g_theRenderer;

Golem::Golem()
{
	m_golemMesh = new SkeletalMesh();

	CreateMeshes();
	CreateSkeleton();

}

Golem::~Golem()
{
	delete m_golemMesh;
	m_golemMesh = nullptr;

	delete m_sphereMesh;
	m_sphereMesh = nullptr;

	delete m_cubeMesh;
	m_cubeMesh = nullptr;
}

void Golem::Update( float deltaSeconds, Transform chestTransform )
{
	m_golemMesh->m_rootBone->m_transform = chestTransform;
}

void Golem::Render()
{
	m_golemMesh->Render();
}

Mat44 Golem::GetHeadMatrix() const
{
	return m_golemMesh->m_headBone->GetRelativeModelMatrix();
}

void Golem::CreateMeshes()
{
	m_sphereMesh = new GPUMesh( g_theRenderer );
	std::vector<Vertex_PCUTBN> sphereVerts;
	std::vector<uint> sphereIndices;
	Vertex_PCUTBN::AppendIndexedVertsSphere( sphereVerts, sphereIndices, 1.f );
	m_sphereMesh->UpdateVertices( sphereVerts );
	m_sphereMesh->UpdateIndices( sphereIndices );

	m_cubeMesh = new GPUMesh( g_theRenderer );
	std::vector<Vertex_PCUTBN> cubeVerts;
	std::vector<uint> cubeIndices;
	Vertex_PCUTBN::AppendIndexedVertsCube( cubeVerts, cubeIndices );
	m_cubeMesh->UpdateVertices( cubeVerts );
	m_cubeMesh->UpdateIndices( cubeIndices );
}

void Golem::CreateSkeleton()
{
	Transform rootTransform;
	Transform headTransform;
	headTransform.m_position.y += 2.f;

	SkeletalMeshBone* root = new SkeletalMeshBone( m_cubeMesh, nullptr, rootTransform );
	SkeletalMeshBone* head = new SkeletalMeshBone( m_sphereMesh, root, headTransform );
	m_golemMesh->m_rootBone = root;
	m_golemMesh->m_headBone = head;

	m_golemMesh->m_rootBone->m_childBones.push_back( head );
}

