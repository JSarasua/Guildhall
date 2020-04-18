#include "Game/Map3D.hpp"
#include "Game/Player3D.hpp"
#include "Game/Companion3D.hpp"

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
	m_player->Update( deltaSeconds );
	m_companion->Update( deltaSeconds );
}

void Map3D::SpawnTiles()
{

}

void Map3D::SpawnEntities()
{

}

Map3D::~Map3D()
{
	delete m_player;
	m_player = nullptr;

	delete m_companion;
	m_companion = nullptr;

	m_game = nullptr;
}

