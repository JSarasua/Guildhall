#include "Game/World.hpp"
#include "Game/Game.hpp"
#include "Game/TileMap.hpp"
#include "Engine/Core/StringUtils.hpp"


World::World( Game* game ) :
	m_game(game)
{

}

void World::Startup()
{
	g_theConsole->PrintString( Rgba8::WHITE, "Loading Maps..." );

	Strings filePaths = GetFileNamesInFolder( "Data/Maps", "*.xml" );

	for( size_t fileIndex = 0; fileIndex < filePaths.size(); fileIndex++ )
	{
		g_theConsole->PrintString( Rgba8::WHITE, Stringf( "Loading Map: %s...", filePaths[fileIndex].c_str() ) );
		
		std::string filePath = "Data/Maps/" + filePaths[fileIndex];
		XmlDocument currentMapDoc		= new XmlDocument;
		XmlElement const& currentMapDefElement = GetRootElement( currentMapDoc, filePath.c_str() );
		Map* newMap = new TileMap( currentMapDefElement, m_game );
		m_maps.push_back( newMap );
	}
	
	m_currentMap = m_maps[0];
}

void World::Shutdown()
{
	for( size_t mapIndex = 0; mapIndex < m_maps.size(); mapIndex++ )
	{
		delete m_maps[mapIndex];
		m_maps[mapIndex] = nullptr;
	}
	//delete m_currentMap;
}

void World::Update( float deltaSeconds )
{
	m_currentMap->Update(deltaSeconds);
}

void World::Render()
{
	m_currentMap->Render();
}

void World::SetCurrentMap( Map* newCurrentMap )
{
	m_currentMap = newCurrentMap;
	//Update cameraPosition;
}

