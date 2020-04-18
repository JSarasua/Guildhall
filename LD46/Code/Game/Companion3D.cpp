#include "Game/Companion3D.hpp"
#include "Game/Game.hpp"
#include "Game/App.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include <vector>

extern RenderContext* g_theRenderer;

Companion3D::Companion3D()
{
	//Make mesh
}

Companion3D::~Companion3D()
{
	delete m_mesh;
	m_mesh = nullptr;
}

void Companion3D::Startup()
{
	m_mesh = new GPUMesh( g_theRenderer );

	std::vector<Vertex_PCUTBN> sphereVerts;
	std::vector<uint> sphereIndices;
	Vertex_PCUTBN::AppendIndexedVertsSphere( sphereVerts, sphereIndices, 1.f );
	m_mesh->UpdateVertices( sphereVerts );
	m_mesh->UpdateIndices( sphereIndices );

	m_transform.m_position = Vec3( 0.f, 15.f, -5.f );
}

void Companion3D::Update( float deltaSeconds )
{
	m_transform.m_position += m_velocity * deltaSeconds;
}

void Companion3D::Render()
{
	Mat44 modelMatrix = m_transform.ToMatrix();
	g_theRenderer->SetModelMatrix( modelMatrix );
	g_theRenderer->DrawMesh( m_mesh );
}

float Companion3D::GetRadius() const
{
	Vec3 scale = m_transform.m_scale;
	float radius = scale.GetLength();
	return radius;
}

Vec3 const& Companion3D::GetPosition() const
{
	return m_transform.m_position;
}

