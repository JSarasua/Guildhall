#include "Game/Map3D.hpp"
#include "Game/Player3D.hpp"
#include "Game/Companion3D.hpp"
#include "Engine/Math/MathUtils.hpp"

Map3D::Map3D()
{
	m_player = new Player3D();
	m_companion = new Companion3D();
}

Map3D::Map3D( IntVec2 const& mapSize, Game* game ) : 
	m_game( game ),
	m_mapSize( mapSize )
{
	m_player = new Player3D();
	m_companion = new Companion3D();
}

void Map3D::Startup()
{
	m_companion->Startup();
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
	m_companion->Render();
}

void Map3D::UpdateTiles( float deltaSeconds )
{

}

void Map3D::UpdateEntities( float deltaSeconds )
{
	AddGravity( deltaSeconds );
	CheckForCollisions();

	m_player->Update( deltaSeconds );
	m_companion->Update( deltaSeconds );

}

void Map3D::AddGravity( float deltaSeconds )
{
	float gravity = 10.f * deltaSeconds;
	float playerPositionY = m_player->GetPosition().y;
	float companionPositionY = m_companion->GetPosition().y;

	if( playerPositionY > 0.f )
	{
		m_player->m_velocity.y -= gravity;
	}

	if( companionPositionY > 0.f )
	{
		m_companion->m_velocity.y -= gravity;
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
	float companionRadius = m_companion->GetRadius();
	float playerRadius = m_player->GetRadius();

	Vec3 companionPosition = m_companion->GetPosition();
	Vec3 playerPosition = m_player->GetPosition();

	float playerToCompanionLen = GetDistance3D( companionPosition, playerPosition );

	if( playerToCompanionLen < (companionRadius + playerRadius) )
	{
		Vec3 playerToCompanion = companionPosition - playerPosition;
		playerToCompanion.Normalize();
		Vec3 velocityToAdd = playerToCompanion * 10.f;

		m_companion->m_velocity += velocityToAdd;
	}

	if( playerPosition.y <= 0.f )
	{
		m_player->m_velocity.y = Max( 0.f, m_player->m_velocity.y );
		m_player->m_transform.m_position.y = 0.f;
	}

	if( companionPosition.y <= 0.f )
	{
		m_companion->m_velocity.y = Max( 0.f, m_companion->m_velocity.y );
		m_player->m_transform.m_position.y = 0.f;
	}
}

Map3D::~Map3D()
{
	delete m_player;
	m_player = nullptr;

	delete m_companion;
	m_companion = nullptr;

	m_game = nullptr;
}

