#include "Game/Companion3D.hpp"
#include "Game/Game.hpp"
#include "Game/App.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include <vector>

extern RenderContext* g_theRenderer;

Companion3D::Companion3D( RandomNumberGenerator* rand )
{
	m_rand = rand;
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

	m_dissolveTimer.SetSeconds( 1.0 );
	m_respawnTimer.SetSeconds( 2.0 );

	m_transform.SetUniformScale( 3.f );

	Respawn();
	//m_transform.m_position = Vec3( 0.f, 15.f, -5.f );
}

void Companion3D::Update( float deltaSeconds )
{
	if( !(m_dissolveTimer.IsRunning() || m_respawnTimer.IsRunning()) )
	{
		m_transform.m_position += m_velocity * deltaSeconds;
		m_velocity -= m_velocity * ( 0.1f * deltaSeconds );
	}

	CheckForDissolve();
	CheckForRespawn();
}

void Companion3D::Render()
{
	if( !(m_dissolveTimer.IsRunning() || m_respawnTimer.IsRunning()) )
	{
		Mat44 modelMatrix = m_transform.ToMatrix();
		g_theRenderer->SetModelMatrix( modelMatrix );
		g_theRenderer->DrawMesh( m_mesh );
	}
}

void Companion3D::Respawn()
{
	float xPos = m_rand->RollRandomFloatInRange( -40.f, 40.f );
	float yPos = m_rand->RollRandomFloatInRange( 50.f, 200.f );
	float zPos = m_rand->RollRandomFloatInRange( -40.f, 40.f );

	Vec3 newPos = Vec3( xPos, yPos, zPos );
	m_transform.m_position = newPos;
	m_velocity = Vec3();

	m_dissolveTimer.Stop();
	m_respawnTimer.Stop();
}

void Companion3D::ActivateDissolve()
{
	m_dissolveTimer.Reset();
}

void Companion3D::CheckForDissolve()
{
	if( m_dissolveTimer.IsRunning() && m_dissolveTimer.HasElapsed() )
	{
		m_dissolveTimer.Stop();
		m_respawnTimer.Reset();
	}
}

void Companion3D::CheckForRespawn()
{
	if( m_respawnTimer.IsRunning() && m_respawnTimer.HasElapsed() )
	{
		Respawn();
	}
}

bool Companion3D::IsDissolving()
{
	return m_dissolveTimer.IsRunning();
}

bool Companion3D::IsRespawning()
{
	return m_respawnTimer.IsRunning();
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

