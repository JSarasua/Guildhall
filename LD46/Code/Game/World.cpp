#include "Game/World.hpp"
#include "Game/Game.hpp"



World::World( Game* game ) :
	m_game(game)
{

}

void World::Startup()
{
	//Create 
	m_currentMap = new Map(IntVec2(INT_MAP_SIZE_X,INT_MAP_SIZE_Y), m_game);
	m_currentMap->Startup();
}

void World::Shutdown()
{
	delete m_currentMap;
}

void World::Update( float deltaSeconds )
{
	m_currentMap->Update(deltaSeconds);
}

void World::Render()
{
	m_currentMap->Render();
}

Player* World::GetPlayer()
{
	return m_currentMap->GetPlayer();
}

IntVec2 World::getCurrentMapBounds() const
{
	return m_currentMap->GetMapBounds();
}

