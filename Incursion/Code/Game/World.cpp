#include "Game/World.hpp"
#include "Game/Game.hpp"



World::World( Game* game ) :
	m_game(game)
{

}

void World::Startup()
{
	//Create 
	m_currentMap = new Map(IntVec2(INT_MAP_SIZE_X_1,INT_MAP_SIZE_Y_1), LEVELONE, m_game);
	m_currentMap->Startup();

	m_maps.push_back(m_currentMap);
	m_maps.push_back(new Map(IntVec2(INT_MAP_SIZE_X_2,INT_MAP_SIZE_Y_2), LEVELTWO, m_game));
	m_maps.push_back(new Map(IntVec2(INT_MAP_SIZE_X_3,INT_MAP_SIZE_Y_3), LEVELTHREE, m_game));
}

void World::Shutdown()
{
	delete m_currentMap;
}

void World::Update( float deltaSeconds )
{
	if( m_currentMap->ShouldGoToNextLevel() && m_currentMapIndex + 1 < m_maps.size() )
	{
		Player* player = GetPlayer();
		int currentLives = GetCurrentLives();

		m_GoToNextMap = false;
		m_currentMapIndex++;
		m_currentMap = m_maps[m_currentMapIndex];
		m_currentMap->Startup(player, currentLives);
	}
	else if( m_currentMap->ShouldGoToNextLevel() && (m_currentMapIndex+1) >= m_maps.size() )
	{
		m_isGameCompleted = true;
	}

	m_currentMap->Update(deltaSeconds);
}

void World::Render()
{
	m_currentMap->Render();
}

void World::TriggerNextMap()
{
	m_GoToNextMap = true;
}

bool World::IsGameCompleted() const
{
	return m_isGameCompleted;
}

Player* World::GetPlayer()
{
	return m_currentMap->GetPlayer();
}

int World::GetCurrentLives()
{
	return m_currentMap->GetCurrentLives();
}

IntVec2 World::getCurrentMapBounds() const
{
	return m_currentMap->GetMapBounds();
}

