#include "Game/Map3D.hpp"
#include "Game/Player3D.hpp"
#include "Game/Companion3D.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/MeshUtils.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Core/Vertex_PCUTBN.hpp"

extern RenderContext* g_theRenderer;

Map3D::Map3D()
{
	m_player = new Player3D();

	m_companionMesh = new GPUMesh( g_theRenderer );
	std::vector<Vertex_PCUTBN> vertexes;
	std::vector<uint> indices;
	MeshImportOptions_t options;
	options.m_transform.SetUniformScale( 0.1f );
	LoadOBJToVertexArray( vertexes, indices, "Data/Meshes/teapot.obj", options );

	m_companionMesh->UpdateVertices( vertexes );
	m_companionMesh->UpdateIndices( indices );

	for( size_t companionIndex = 0; companionIndex < 30; companionIndex++ )
	{
		m_companions.push_back( new Companion3D( &m_game->m_rand, m_companionMesh ) );
	}
}

Map3D::Map3D( IntVec2 const& mapSize, Game* game ) : 
	m_game( game ),
	m_mapSize( mapSize )
{
	m_player = new Player3D();

	m_companionMesh = new GPUMesh( g_theRenderer );
	std::vector<Vertex_PCUTBN> vertexes;
	std::vector<uint> indices;
	MeshImportOptions_t options;
	options.m_transform.SetUniformScale( 0.1f );
	LoadOBJToVertexArray( vertexes, indices, "Data/Meshes/teapot.obj", options );

	m_companionMesh->UpdateVertices( vertexes );
	m_companionMesh->UpdateIndices( indices );

	for( size_t companionIndex = 0; companionIndex < 30; companionIndex++ )
	{
		m_companions.push_back( new Companion3D( &m_game->m_rand, m_companionMesh ) );
	}

}

void Map3D::Startup()
{
	for( size_t companionIndex = 0; companionIndex < m_companions.size(); companionIndex++ )
	{
		m_companions[companionIndex]->Startup();
	}
}

void Map3D::Shutdown()
{

}

void Map3D::Update( float deltaSeconds )
{
	UpdateTiles( deltaSeconds );
	UpdateEntities( deltaSeconds );
}

void Map3D::Render()
{
	RenderTiles();
	RenderEntities();
}

void Map3D::RenderTiles()
{

}

void Map3D::RenderEntities()
{
	m_player->Render();

	for( size_t companionIndex = 0; companionIndex < m_companions.size(); companionIndex++ )
	{
		m_companions[companionIndex]->Render();
	}
}

void Map3D::UpdateTiles( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

void Map3D::UpdateEntities( float deltaSeconds )
{
	AddGravity( deltaSeconds );
	CheckForCollisions();

	m_player->Update( deltaSeconds );

	for( size_t companionIndex = 0; companionIndex < m_companions.size(); companionIndex++ )
	{
		m_companions[companionIndex]->Update( deltaSeconds );
	}
}

void Map3D::AddGravity( float deltaSeconds )
{
	float gravity = 75.f * deltaSeconds;
	float playerPositionY = m_player->GetPosition().y;


	if( playerPositionY > m_player->GetRadius() )
	{
		m_player->m_velocity.y -= gravity;
	}

	for( size_t companionIndex = 0; companionIndex < m_companions.size(); companionIndex++ )
	{
		float companionPositionY = m_companions[companionIndex]->GetPosition().y;

		if( companionPositionY > 0.f )
		{
			m_companions[companionIndex]->m_velocity.y -= 0.25f * gravity;
		}
	}


}

void Map3D::SpawnTiles()
{

}

void Map3D::SpawnEntities()
{

}

void Map3D::CheckForCollisions()
{
	float playerRadius = m_player->GetRadius();
	Vec3 playerPosition = m_player->GetPosition();

	if( playerPosition.y <= m_player->GetRadius() )
	{
		m_player->m_velocity.y = Max( 0.f, m_player->m_velocity.y );
		//Vec3 playerPosition = m_player->GetPosition();
		playerPosition.y = m_player->GetRadius();
		m_player->SetPosition( playerPosition );
	}

	for( size_t companionIndex = 0; companionIndex < m_companions.size(); companionIndex++ )
	{
		Vec3 companionPosition = m_companions[companionIndex]->m_transform.m_position;
		float companionRadius = m_companions[companionIndex]->GetRadius();
		float playerToCompanionLen = GetDistance3D( companionPosition, playerPosition );

		if( playerToCompanionLen < (companionRadius + playerRadius) )
		{
			if( !(m_companions[companionIndex]->IsDissolving() || m_companions[companionIndex]->IsRespawning()) )
			{
				m_companions[companionIndex]->ActivateDissolve();
				m_companions[companionIndex]->m_velocity.y = Max( 0.f, m_companions[companionIndex]->m_velocity.y );
				m_companions[companionIndex]->m_transform.m_position.y = 0.f;
				
				m_player->m_golemScale += 1.f / m_player->m_golemScale;
			}

// 			Vec3 playerToCompanion = companionPosition - playerPosition;
// 			playerToCompanion.Normalize();
// 			Vec3 velocityToAdd = playerToCompanion * 10.f;
// 			velocityToAdd.y += 5.f;
// 
// 			m_companions[companionIndex]->m_velocity += velocityToAdd;
		}

		float yCompanionPosition = m_companions[companionIndex]->GetPosition().y;
		if( yCompanionPosition <= 0.f )
		{
			if( !( m_companions[companionIndex]->IsDissolving() || m_companions[companionIndex]->IsRespawning() ) )
			{
				m_companions[companionIndex]->ActivateDissolve();
				m_companions[companionIndex]->m_velocity.y = Max( 0.f, m_companions[companionIndex]->m_velocity.y );
				m_companions[companionIndex]->m_transform.m_position.y = 0.f;
			}
		}
	}


}

Map3D::~Map3D()
{
	delete m_player;
	m_player = nullptr;

	delete m_companionMesh;
	m_companionMesh = nullptr;

	for( size_t companionIndex = 0; companionIndex < m_companions.size(); companionIndex++ )
	{
		delete m_companions[companionIndex];
		m_companions[companionIndex] = nullptr;
	}

	m_game = nullptr;
}

