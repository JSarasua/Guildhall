#include "Game/World.hpp"
#include "Game/Game.hpp"



World::World( Game* game ) :
	m_game(game)
{

}

void World::Startup()
{
	//Create 
	//m_currentMap = new Map(IntVec2(INT_MAP_SIZE_X,INT_MAP_SIZE_Y), m_game);
	m_currentMap = new Map("Island");
	m_currentMap->AddPlayer( nullptr );
	m_currentMap->Startup();
	m_maps.push_back( m_currentMap );
}

void World::Shutdown()
{
	m_currentMap->DeletePlayer();
	for( size_t mapIndex = 0; mapIndex < m_maps.size(); mapIndex++ )
	{
		delete m_maps[mapIndex];
		m_maps[mapIndex] = nullptr;
	}

	m_currentMap = nullptr;
}

void World::Update( float deltaSeconds )
{
	m_currentMap->Update(deltaSeconds);
}

void World::Render()
{
	m_currentMap->Render();
}

void World::RenderDebug() const
{
	m_currentMap->RenderDebugMode();
}

void World::MoveToNextMap()
{
	Actor* player = GetPlayer();

	m_currentMapIndex++;
	if( m_currentMapIndex >= m_maps.size() )
	{
		m_currentMapIndex = 0;
	}

	m_currentMap = m_maps[m_currentMapIndex];
	m_currentMap->AddPlayer( player );
}

Actor* World::GetPlayer()
{
	return m_currentMap->GetPlayer();
}

void World::GetPlayers( std::vector<Actor*>& players )
{
	m_currentMap->GetPlayers(players);
}

IntVec2 World::getCurrentMapBounds() const
{
	return m_currentMap->GetMapBounds();
}

