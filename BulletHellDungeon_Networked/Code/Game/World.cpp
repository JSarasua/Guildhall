#include "Game/World.hpp"
#include "Game/Game.hpp"
#include "Game/EntityFactory.hpp"


World::World( Game* game ) :
	m_game(game)
{

}

void World::Startup()
{
	//Create 
	//m_currentMap = new Map(IntVec2(INT_MAP_SIZE_X,INT_MAP_SIZE_Y), m_game);
	m_currentMap = new Map("Level1");
	m_currentMap->SpawnSpawnersLevel1();
	m_currentMap->AddPlayer( nullptr, 0 );
	m_currentMap->Startup();
	m_maps.push_back( m_currentMap );
	
	Map* level2 = new Map("Level2");
	level2->SpawnSpawnersLevel2();
	level2->Startup();
	m_maps.push_back( level2 );

// 	Map* level3 = new Map( "Level3" );
// 	level3->SpawnSpawnersLevel3();
// 	level3->Startup();
// 	m_maps.push_back( level3 );
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

	//Check if boss is dead

	if( m_currentMap->IsBossDead() )
	{
		MoveToNextMap();
	}
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
	std::vector<Actor*> players;
	GetPlayers( players );
// 	m_maps[m_currentMapIndex]->Shutdown();
// 	delete m_maps[m_currentMapIndex];
// 	m_maps[m_currentMapIndex] = nullptr;

	int newMapIndex = m_currentMapIndex + 1;
	if( newMapIndex >= m_maps.size() )
	{
		g_theGame->TriggerVictoryState();
	}
	else
	{
		m_currentMapIndex = newMapIndex;
		m_currentMap = m_maps[m_currentMapIndex];

		for( size_t playerIndex = 0; playerIndex < players.size(); playerIndex++ )
		{
			m_currentMap->AddPlayer( players[playerIndex], (int)playerIndex );
		}
	}
}

Actor* World::GetPlayer()
{
	return m_currentMap->GetPlayer();
}

void World::GetPlayers( std::vector<Actor*>& players )
{
	m_currentMap->GetPlayers(players);
}

int World::GetBossHealth() const
{
	return m_currentMap->GetBossHealth();
}

void World::AddNewPlayer( int playerSlot )
{
	m_currentMap->AddPlayer( nullptr, playerSlot );
}

void World::CreateEntity( CreateEntityMessage const& createMessage )
{
	if( createMessage.entityType == ID_PLAYER )
	{
		AddNewPlayer( createMessage.entityID );
	}
	else
	{
		Entity* entity = EntityFactory::CreateEntity( createMessage );
		m_currentMap->SpawnEntity( entity );
	}

}

void World::UpdateEntity( UpdateEntityMessage const& updateMessage )
{
	m_currentMap->UpdateEntity( updateMessage );
}

void World::DeleteEntity( DeleteEntityMessage const& deleteMessage )
{
	m_currentMap->DeleteEntity( deleteMessage );
}

IntVec2 World::GetCurrentMapBounds() const
{
	return m_currentMap->GetMapBounds();
}

