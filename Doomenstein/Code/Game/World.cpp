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
		
		m_maps[filePaths[fileIndex]] = newMap;
	}
	
	m_currentMap = m_maps.begin()->second;
	m_currentMap->SetPlayerToStart();


	g_theEventSystem->SubscribeMethodToEvent("warp", CONSOLECOMMAND, this, &World::WarpPlayer );
}

void World::Shutdown()
{
	m_maps.clear();
// 	for( size_t mapIndex = 0; mapIndex < m_maps.size(); mapIndex++ )
// 	{
// 		delete m_maps[mapIndex];
// 		m_maps[mapIndex] = nullptr;
// 	}
	//delete m_currentMap;
}

void World::Update( float deltaSeconds )
{
	if( m_currentMap && m_currentMap->IsValid() )
	{
		m_currentMap->Update(deltaSeconds);
	}

}

void World::Render()
{
	if( m_currentMap && m_currentMap->IsValid() )
	{
		m_currentMap->Render();
	}

}

void World::SetCurrentMap( Map* newCurrentMap )
{
	m_currentMap = newCurrentMap;
	//Update cameraPosition;
}

bool World::WarpPlayer( EventArgs const& args )
{
	if( !args.IsNotEmpty() )
	{
		g_theConsole->PrintString( Rgba8::YELLOW, "Valid Warp commands:");
		g_theConsole->PrintString( Rgba8::GREEN, "warp pos=1.5,2.5");
		g_theConsole->PrintString( Rgba8::GREEN, "warp map=MAPNAME.xml");
		g_theConsole->PrintString( Rgba8::GREEN, "warp map=MAPNAME.xml pos=2.5,3.7 yaw=30");
		g_theConsole->PrintString( Rgba8::YELLOW, "Valid maps:");

		for( auto mapIter = m_maps.begin(); mapIter != m_maps.end(); mapIter++ )
		{
			g_theConsole->PrintString( Rgba8::GREEN, Stringf( "%s", mapIter->first.c_str() ) );
		}

	}
	else
	{
		std::string mapName = args.GetValue( "map", "" );
		Vec2 position = args.GetValue( "pos", Vec2( -999.f, -999.f ) );
		float yaw = args.GetValue( "yaw", -999.f );

		if( mapName != "" )
		{
			//mapName.append( ".xml" );
			auto mapIter = m_maps.find( mapName );
			if( mapIter == m_maps.end() )
			{
				g_theConsole->ErrorString( Stringf( "%s not found in maps", mapName.c_str() ) );
			}
			else
			{
				g_theConsole->PrintString( Rgba8::GREEN, Stringf( "Warping to %s...", mapIter->first.c_str() ) );
				m_currentMap = mapIter->second;
			}

			//warp to map
		}
		if( position == Vec2( -999.f, -999.f ) )
		{

			position = m_currentMap->GetPlayerStartPosition();
		}
		if( yaw == -999.f )
		{
			yaw = m_currentMap->GetPlayerStartYaw();
		}

		g_theGame->SetPlayerPosition( Vec3( position, 0.5f ) );
		g_theGame->SetPlayerRotation( Vec3( 0.f, 0.f, yaw ) );
	}



	return true;
}

