#include "Game/World.hpp"
#include "Game/Game.hpp"
#include "Game/TileMap.hpp"



World::World( Game* game ) :
	m_game(game)
{

}

void World::Startup()
{
	XmlDocument singleBlockMap		= new XmlDocument;
	XmlElement const& singleBlockMapDef = GetRootElement( singleBlockMap, "Data/Maps/TwistyMaze.xml");
	//Create 
	m_currentMap = new TileMap( singleBlockMapDef, m_game );
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

