#include "Game/Player3D.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Game/Game.hpp"
#include "Game/App.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include <vector>

extern RenderContext* g_theRenderer;

Player3D::Player3D()
{
	m_mesh = new GPUMesh( g_theRenderer );

	std::vector<Vertex_PCUTBN> sphereVerts;
	std::vector<uint> sphereIndices;
	Vertex_PCUTBN::AppendIndexedVertsSphere( sphereVerts, sphereIndices, 1.f );
	m_mesh->UpdateVertices( sphereVerts );
	m_mesh->UpdateIndices( sphereIndices );

	m_transform.m_position = Vec3( 0.f, 4.f, 1.f );
}

Player3D::~Player3D()
{
	delete m_mesh;
	m_mesh = nullptr;
}

void Player3D::Update( float deltaSeconds )
{

}

void Player3D::Render()
{
	Mat44 modelMatrix = m_transform.ToMatrix();
	g_theRenderer->SetModelMatrix( modelMatrix );
	g_theRenderer->DrawMesh( m_mesh );
}

