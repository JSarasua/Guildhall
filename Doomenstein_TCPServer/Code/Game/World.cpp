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
		Map* newMap = new TileMap( currentMapDefElement );
		newMap->m_name = filePaths[fileIndex];
		
		m_maps[filePaths[fileIndex]] = newMap;
	}
	std::string startMapStr = g_gameConfigBlackboard->GetValue("startMap", "INVALID" );
	bool hasValidStartMap = g_theConsole->GuaranteeOrError( startMapStr != "INVALID", "Gameconfig.xml did not have a startmap" );
	
	if( hasValidStartMap )
	{
		auto mapIter = m_maps.find( startMapStr );
		hasValidStartMap =g_theConsole->GuaranteeOrError( mapIter != m_maps.end(), Stringf( "Couldn't find start map: %s", startMapStr.c_str() ) );
		
		if( hasValidStartMap )
		{
			Map* startMap = mapIter->second;
			g_theConsole->PrintString( Rgba8::GREEN, Stringf( "Warping player to %s...", startMapStr.c_str() ) );
			m_currentMap = startMap;
		}
		else
		{
			g_theConsole->PrintString( Rgba8::GREEN, Stringf( "Warping player to %s...", m_maps.begin()->first.c_str() ) );
			m_currentMap = m_maps.begin()->second;
		}
	}
	else
	{
		g_theConsole->PrintString( Rgba8::GREEN, Stringf( "Warping player to %s...", m_maps.begin()->first.c_str() ) );
		m_currentMap = m_maps.begin()->second;
	}

	if( m_currentMap )
	{
		m_currentMap->SetPlayerToStart();
	}



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

void World::WarpPlayerWithEntity( Entity* entityToWarp, std::string const& destMap, Vec2 const& destPos, float destYawOffset )
{
	//Update Position
	Vec3 entityPosition = entityToWarp->GetPosition();
	entityPosition.x = destPos.x;
	entityPosition.y = destPos.y;
	entityToWarp->SetPosition( entityPosition );

	Vec3 entityRotation = entityToWarp->GetRotationPitchRollYawDegrees();
	entityToWarp->RotatePitchRollYawDegrees( Vec3( 0.f, 0.f, destYawOffset ) );

	if( destMap != "" )
	{
		//mapName.append( ".xml" );
		auto mapIter = m_maps.find( destMap );
		if( mapIter == m_maps.end() )
		{
			g_theConsole->ErrorString( Stringf( "%s not found in maps", destMap.c_str() ) );
		}
		else
		{
			m_currentMap->RemoveEntity( entityToWarp );
			g_theConsole->PrintString( Rgba8::GREEN, Stringf( "Warping to %s...", mapIter->first.c_str() ) );
			m_currentMap = mapIter->second;
			m_currentMap->AddEntity( entityToWarp );
		}

		//warp to map
	}





}

Entity* World::GetClosestEntityInSector( Vec3 const& position, Vec2 const& forwardVector, float forwardSpread, float maxDistance )
{
	return m_currentMap->GetClosestEntityInSector( position, forwardVector, forwardSpread, maxDistance );
}

Map* World::GetMapByName( std::string const& mapName )
{
	Map* mapToGet = nullptr;
	auto mapIter = m_maps.find( mapName );
	if( mapIter != m_maps.end() )
	{
		mapToGet = mapIter->second;
	}

	return mapToGet;
}

