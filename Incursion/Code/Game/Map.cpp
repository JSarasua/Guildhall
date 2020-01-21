#include "Map.hpp"
#include "Tile.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Boulder.hpp"
#include "Game/Bullet.hpp"
#include "Game/NpcTank.hpp"
#include "Game/NpcTurret.hpp"
#include "Game/Player.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/App.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Game/Explosion.hpp"
#include "Engine/Audio/AudioSystem.hpp"


extern App* g_theApp;
extern RenderContext* g_theRenderer;
extern InputSystem* g_theInput;
extern AudioSystem* g_theAudio;


Map::Map() : 
	m_mapSize(0,0)
{}

Map::Map(const IntVec2& mapSize, Level level, Game* game) :
	m_mapSize(mapSize),
	m_level(level),
	m_game(game)
{
	m_NumOftiles = m_mapSize.x * m_mapSize.y;

	m_tileTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Terrain_8x8.png" );
	m_tileDefinition.InitializeTileDefinitions( *m_tileTexture );

	SpawnTiles();
	SpawnEntities();
}

Map::~Map()
{

}


void Map::Startup()
{
	m_PlayerLives = PLAYERLIVES;



	for( int tileIndex = 0; tileIndex < m_tiles.size(); tileIndex++ )
	{
		m_tiles[tileIndex].AppendVerts(m_vertsToRender);
	}


	CreateExplosionAnimation();
}

void Map::Startup( Player* currentPlayer, int currentLives )
{
	m_PlayerLives = currentLives;

	m_tileTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Terrain_8x8.png" );
	m_tileDefinition.InitializeTileDefinitions( *m_tileTexture );

	for( int tileIndex = 0; tileIndex < m_tiles.size(); tileIndex++ )
	{
		m_tiles[tileIndex].AppendVerts( m_vertsToRender );
	}
	
	SetPlayer(currentPlayer);
	CreateExplosionAnimation();
}

void Map::Shutdown()
{

}


void Map::Update( float deltaSeconds )
{
	UpdateTiles(deltaSeconds);
	UpdateEntities(deltaSeconds);
	CheckIfPlayerExited();

	GarbageCollectEntities();

	CheckInput();
}

void Map::Render()
{
	RenderTiles();
	RenderEntities();

	RenderRaycasts();
}


Player* Map::GetPlayer()
{
	//return static_cast<Player*>(m_entities[0]);
	return static_cast<Player*>(m_EntityListsByType[ENTITY_TYPE_PLAYER][0]);
}

bool Map::ShouldGoToNextLevel() const
{
	return m_GoToNextLevel;
}

IntVec2 Map::GetMapBounds() const
{
	return m_mapSize;
}

void Map::SpawnNewEntity( EntityType type, EntityFaction faction, const Vec2& spawnPosition, float orientationDegrees, float size /*= 1.f*/)
{
	Entity* newEntity;
	newEntity = nullptr;

	switch( type )
	{
// 	case ENTITY_TYPE_INVALID:
// 		break;
	case ENTITY_TYPE_PLAYER: newEntity = new Player(spawnPosition, type, faction, m_game);
		break;
	case ENTITY_TYPE_NPC_TURRET: newEntity = new NpcTurret(spawnPosition, orientationDegrees, type, faction, m_game);
		break;
	case ENTITY_TYPE_NPC_TANK: newEntity = new NpcTank(spawnPosition, orientationDegrees, type, faction, m_game);
		break;
	case ENTITY_TYPE_BOULDER: newEntity = new Boulder(spawnPosition, orientationDegrees, type, faction, m_game);
		break;
	case ENTITY_TYPE_GOOD_BULLET: newEntity = new Bullet(spawnPosition, orientationDegrees, type, faction, m_game);
		break;
	case ENTITY_TYPE_EVIL_BULLET: newEntity = new Bullet(spawnPosition, orientationDegrees, type, faction, m_game);
		break;
	case ENTITY_TYPE_EXPLOSION: newEntity = new Explosion(spawnPosition, orientationDegrees, size, type, faction, *m_explosionAnimDef, m_game);
		break;
	default: ERROR_RECOVERABLE( Stringf("Attempted to create an entity that doesn't exist\n"));	
		break;
	}

	AddEntityToMap( newEntity );

	//m_EntityListsByType[type]
}

int Map::GetCurrentLives()
{
	return m_PlayerLives;
}

void Map::SetPlayer( Player* currentPlayer )
{
	Player* oldPlayer = static_cast<Player*>(m_EntityListsByType[ENTITY_TYPE_PLAYER][0]);
	Vec2 oldPosition = oldPlayer->GetPosition();
	//delete oldPlayer;
	m_EntityListsByType[ENTITY_TYPE_PLAYER][0] = currentPlayer;
	currentPlayer->SetPosition(oldPosition);

}

void Map::CheckIfPlayerExited()
{
	Player* player = GetPlayer();
	
	if( player->IsGarbage() )
	{
		return;
	}

	const IntVec2 playerPostionInt( player->GetPosition() );
	Vec2 playerPosition( player->GetPosition() );


	static const IntVec2 displacement8[] =
	{
		IntVec2( -1, 0 ),	//left
		IntVec2( 1, 0 ),	//right
		IntVec2( 0, 1 ),	//up
		IntVec2( 0, -1 ),	//down
		IntVec2( -1, 1 ),	//up left
		IntVec2( 1, 1 ),	//up right
		IntVec2( -1, -1 ),	//down left
		IntVec2( 1, -1 ),	//down right
	};

	for( int positionIndex = 0; positionIndex < 8; positionIndex++ )
	{
		const IntVec2 currentPosition = playerPostionInt + displacement8[positionIndex];
		if( m_tiles[GetTileIndexForTileCoordinates( currentPosition )].m_tileType == EXIT )
		{
			AABB2 tileAABB2 = m_tiles[GetTileIndexForTileCoordinates( currentPosition )].GetAABB2();
			if( DoDiscAndAABBOverlap2D( playerPosition, player->m_physicsRadius, tileAABB2 ) )
			{
				m_GoToNextLevel = true;
			}

		}
	}
}

void Map::CreateExplosionAnimation()
{
	Texture* explosionTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Explosion_5x5.png");
	SpriteSheet* explosionSheet = new SpriteSheet(*explosionTexture, IntVec2(5,5));
	m_explosionAnimDef = new SpriteAnimDefinition(*explosionSheet, 0, 24, EXPLOSION_DURATION, SpriteAnimPlaybackType::PINGPONG);
}

IntVec2 Map::GetRandomValidAdjacentTilePosition( const IntVec2& tilePosition ) const
{


	int randomDirection = m_game->m_rand.RollRandomIntInRange(0,3);
	IntVec2 newPosition(0,0);

	switch( randomDirection )
	{
	case Left:	newPosition = tilePosition + IntVec2(-1,0);
		break;
	case Right:	newPosition = tilePosition + IntVec2(1,0);
		break;
	case Up:	newPosition = tilePosition + IntVec2(0,1);
		break;
	case Down:	newPosition = tilePosition + IntVec2(0,-1);
		break;
	default:
		break;
	}


	while( !IsValidTileCoordinates( newPosition ) )
	{
		randomDirection = m_game->m_rand.RollRandomIntInRange( 0, 3 );

		switch( randomDirection )
		{
		case Left:	newPosition = tilePosition + IntVec2( -1, 0 );
			break;
		case Right:	newPosition = tilePosition + IntVec2( 1, 0 );
			break;
		case Up:	newPosition = tilePosition + IntVec2( 0, 1 );
			break;
		case Down:	newPosition = tilePosition + IntVec2( 0, -1 );
			break;
		default:
			break;
		}
	}

	return newPosition;
}

void Map::RebuildMap()
{
	switch( m_level )
	{
	case LEVELONE: RebuildLevelOne();
		break;
	case LEVELTWO: RebuildLevelTwo();
		break;
	case LEVELTHREE: RebuildLevelThree();
		break;
	default:
		break;
	}

}

bool Map::ValidateTileMap( IntVec2 startPosition, IntVec2 exitPosition )
{
	//return true;

	std::vector<int> validMapTiles;
	
	validMapTiles.resize(m_tiles.size());

	for( int validMapTilesIndex = 0; validMapTilesIndex < validMapTiles.size(); validMapTilesIndex++ )
	{
		validMapTiles[validMapTilesIndex] = -1;
	}




	int startingTileIndex = GetTileIndexForTileCoordinates( startPosition );
	//Check yourself
	if( IsNonSolidTileCoordinates( startPosition ) )
	{
		validMapTiles[startingTileIndex] = 1;
		m_tiles[startingTileIndex].m_validTile = 1;
	}
	else
	{
		validMapTiles[startingTileIndex] = 0;
		m_tiles[startingTileIndex].m_validTile = 0;
	}

	int maxSteps = 10000;
	FloodFill( startPosition, maxSteps, validMapTiles );

	int exitPositionTileIndex = GetTileIndexForTileCoordinates(exitPosition);
	if( validMapTiles[exitPositionTileIndex] == 1 )
	{
		return true;
	}
	return false;
}

void Map::SpawnLevelOneTiles()
{
	int x = 0;
	int y = 0;


	for( int mapIndex = 0; mapIndex < m_NumOftiles; mapIndex++ )
	{
		y = mapIndex / (m_mapSize.x);
		x = mapIndex - y * m_mapSize.x;

		m_tiles.push_back( Tile( IntVec2( x, y ), GRAVEL, mapIndex ) );

	}


	int randomWormLength = 0;

	for( int wormIndex = 0; wormIndex < WORMCOUNT; wormIndex++ )
	{
		randomWormLength = m_game->m_rand.RollRandomIntInRange( 0, MAXWORMLENGTH );
		SpawnWorm( BRICK, randomWormLength );
	}


	//Create Safe zone
	for( int safeZoneIndexX = 1; safeZoneIndexX < 6; safeZoneIndexX++ )
	{
		for( int safeZoneIndexY = 1; safeZoneIndexY < 6; safeZoneIndexY++ )
		{
			m_tiles[GetTileIndexForTileCoordinates( IntVec2( safeZoneIndexX, safeZoneIndexY ) )].m_tileType = GRAVEL;
		}
	}
	for( int safeZoneIndexX = m_mapSize.x - 1; safeZoneIndexX > m_mapSize.x - 7; safeZoneIndexX-- )
	{
		for( int safeZoneIndexY = m_mapSize.y - 1; safeZoneIndexY > m_mapSize.y - 7; safeZoneIndexY-- )
		{
			m_tiles[GetTileIndexForTileCoordinates( IntVec2( safeZoneIndexX, safeZoneIndexY ) )].m_tileType = GRAVEL;
		}
	}

	IntVec2 startPosition = Vec2( 2, 2 );
	IntVec2 exitPosition = IntVec2( m_mapSize.x - 3, m_mapSize.y - 3 );



	//Add exit Tile
	m_tiles[GetTileIndexForTileCoordinates( IntVec2( m_mapSize.x - 3, m_mapSize.y - 3 ) )].m_tileType = EXIT;




	//Border wall
	int tileIndex = 0;
	for( int borderXIndex= 0; borderXIndex< m_mapSize.x; borderXIndex++ )
	{
		m_tiles[borderXIndex].m_tileType = BRICK;

		tileIndex = m_mapSize.x * (m_mapSize.y - 1) + borderXIndex;
		m_tiles[tileIndex].m_tileType = BRICK;
	}
	for( int borderYIndex= 0; borderYIndex < m_mapSize.y; borderYIndex++ )
	{
		tileIndex = borderYIndex * m_mapSize.x;
		m_tiles[tileIndex].m_tileType = BRICK;

		tileIndex = borderYIndex * m_mapSize.x + (m_mapSize.x-1);
		m_tiles[tileIndex].m_tileType = BRICK;
	}



	if( !ValidateTileMap( startPosition, exitPosition ) )
	{
		RebuildMap();
	}

	//Unreachable tiles are set to the levels Solid type
	for( int unreachableTileIndex = 0; tileIndex < m_tiles.size(); tileIndex++ )
	{
		if( m_tiles[unreachableTileIndex].m_validTile == -1 )
		{
			m_tiles[unreachableTileIndex].m_tileType = BRICK;
		}
	}

}

void Map::SpawnLevelTwoTiles()
{
	int x = 0;
	int y = 0;


	for( int mapIndex = 0; mapIndex < m_NumOftiles; mapIndex++ )
	{
		y = mapIndex / (m_mapSize.x);
		x = mapIndex - y * m_mapSize.x;

		m_tiles.push_back( Tile( IntVec2( x, y ), GRASS, mapIndex ) );

	}


	int randomWormLength = 0;
	for( int wormIndex = 0; wormIndex < WORMCOUNT; wormIndex++ )
	{
		randomWormLength = m_game->m_rand.RollRandomIntInRange( 0, MAXWORMLENGTH );
		SpawnWorm( MUD, randomWormLength );
	}
	for( int wormIndex = 0; wormIndex < WORMCOUNT; wormIndex++ )
	{
		randomWormLength = m_game->m_rand.RollRandomIntInRange( 0, MAXWORMLENGTH );
		SpawnWorm( STONE, randomWormLength );
	}


	//Create Safe zone
	for( int safeZoneIndexX = 1; safeZoneIndexX < 6; safeZoneIndexX++ )
	{
		for( int safeZoneIndexY = 1; safeZoneIndexY < 6; safeZoneIndexY++ )
		{
			m_tiles[GetTileIndexForTileCoordinates( IntVec2( safeZoneIndexX, safeZoneIndexY ) )].m_tileType = GRASS;
		}
	}
	for( int safeZoneIndexX = m_mapSize.x - 1; safeZoneIndexX > m_mapSize.x - 7; safeZoneIndexX-- )
	{
		for( int safeZoneIndexY = m_mapSize.y - 1; safeZoneIndexY > m_mapSize.y - 7; safeZoneIndexY-- )
		{
			m_tiles[GetTileIndexForTileCoordinates( IntVec2( safeZoneIndexX, safeZoneIndexY ) )].m_tileType = GRASS;
		}
	}

	IntVec2 startPosition = Vec2( 2, 2 );
	IntVec2 exitPosition = IntVec2( m_mapSize.x - 3, m_mapSize.y - 3 );



	//Add exit Tile
	m_tiles[GetTileIndexForTileCoordinates( IntVec2( m_mapSize.x - 3, m_mapSize.y - 3 ) )].m_tileType = EXIT;




	//Border wall
	int tileIndex = 0;
	for( int borderXIndex= 0; borderXIndex< m_mapSize.x; borderXIndex++ )
	{
		m_tiles[borderXIndex].m_tileType = STONE;

		tileIndex = m_mapSize.x * (m_mapSize.y - 1) + borderXIndex;
		m_tiles[tileIndex].m_tileType = STONE;
	}
	for( int borderYIndex= 0; borderYIndex < m_mapSize.y; borderYIndex++ )
	{
		tileIndex = borderYIndex * m_mapSize.x;
		m_tiles[tileIndex].m_tileType = STONE;

		tileIndex = borderYIndex * m_mapSize.x + (m_mapSize.x-1);
		m_tiles[tileIndex].m_tileType = STONE;
	}



	if( !ValidateTileMap( startPosition, exitPosition ) )
	{
		RebuildMap();
	}


	//Unreachable tiles are set to the levels Solid type
	for( int unreachableTileIndex = 0; tileIndex < m_tiles.size(); tileIndex++ )
	{
		if( m_tiles[unreachableTileIndex].m_validTile == -1 )
		{
			m_tiles[unreachableTileIndex].m_tileType = STONE;
		}
	}
}

void Map::SpawnLevelThreeTiles()
{
	int x = 0;
	int y = 0;


	for( int mapIndex = 0; mapIndex < m_NumOftiles; mapIndex++ )
	{
		y = mapIndex / (m_mapSize.x);
		x = mapIndex - y * m_mapSize.x;

		m_tiles.push_back( Tile( IntVec2( x, y ), SAND, mapIndex ) );

	}


	int randomWormLength = 0;
	for( int wormIndex = 0; wormIndex < WORMCOUNT; wormIndex++ )
	{
		randomWormLength = m_game->m_rand.RollRandomIntInRange( 0, MAXWORMLENGTH );
		SpawnWorm( LAVA, randomWormLength );
	}
	for( int wormIndex = 0; wormIndex < WORMCOUNT; wormIndex++ )
	{
		randomWormLength = m_game->m_rand.RollRandomIntInRange( 0, MAXWORMLENGTH );
		SpawnWorm( VOLCANICROCK, randomWormLength );
	}


	//Create Safe zone
	for( int safeZoneIndexX = 1; safeZoneIndexX < 6; safeZoneIndexX++ )
	{
		for( int safeZoneIndexY = 1; safeZoneIndexY < 6; safeZoneIndexY++ )
		{
			m_tiles[GetTileIndexForTileCoordinates( IntVec2( safeZoneIndexX, safeZoneIndexY ) )].m_tileType = SAND;
		}
	}
	for( int safeZoneIndexX = m_mapSize.x - 1; safeZoneIndexX > m_mapSize.x - 7; safeZoneIndexX-- )
	{
		for( int safeZoneIndexY = m_mapSize.y - 1; safeZoneIndexY > m_mapSize.y - 7; safeZoneIndexY-- )
		{
			m_tiles[GetTileIndexForTileCoordinates( IntVec2( safeZoneIndexX, safeZoneIndexY ) )].m_tileType = SAND;
		}
	}

	IntVec2 startPosition = Vec2( 2, 2 );
	IntVec2 exitPosition = IntVec2( m_mapSize.x - 3, m_mapSize.y - 3 );



	//Add exit Tile
	m_tiles[GetTileIndexForTileCoordinates( IntVec2( m_mapSize.x - 3, m_mapSize.y - 3 ) )].m_tileType = EXIT;




	//Border wall
	int tileIndex = 0;
	for( int borderXIndex= 0; borderXIndex< m_mapSize.x; borderXIndex++ )
	{
		m_tiles[borderXIndex].m_tileType = VOLCANICROCK;

		tileIndex = m_mapSize.x * (m_mapSize.y - 1) + borderXIndex;
		m_tiles[tileIndex].m_tileType = VOLCANICROCK;
	}
	for( int borderYIndex= 0; borderYIndex < m_mapSize.y; borderYIndex++ )
	{
		tileIndex = borderYIndex * m_mapSize.x;
		m_tiles[tileIndex].m_tileType = VOLCANICROCK;

		tileIndex = borderYIndex * m_mapSize.x + (m_mapSize.x-1);
		m_tiles[tileIndex].m_tileType = VOLCANICROCK;
	}



	if( !ValidateTileMap( startPosition, exitPosition ) )
	{
		RebuildMap();
	}


	//Unreachable tiles are set to the levels Solid type
	for( int unreachableTileIndex = 0; tileIndex < m_tiles.size(); tileIndex++ )
	{
		if( m_tiles[unreachableTileIndex].m_validTile == -1 )
		{
			m_tiles[unreachableTileIndex].m_tileType = VOLCANICROCK;
		}
	}
}

void Map::RebuildLevelOne()
{
	for( int tileIndex = 0; tileIndex < m_tiles.size(); tileIndex++ )
	{
		m_tiles[tileIndex].m_tileType = GRAVEL;
		m_tiles[tileIndex].m_validTile = -1;
	}



	int randomWormLength = 0;
	for( int wormIndex = 0; wormIndex < WORMCOUNT; wormIndex++ )
	{
		randomWormLength = m_game->m_rand.RollRandomIntInRange( 0, MAXWORMLENGTH );
		SpawnWorm( BRICK, randomWormLength );
	}


	//Create Safe zone
	for( int safeZoneIndexX = 1; safeZoneIndexX < 6; safeZoneIndexX++ )
	{
		for( int safeZoneIndexY = 1; safeZoneIndexY < 6; safeZoneIndexY++ )
		{
			m_tiles[GetTileIndexForTileCoordinates( IntVec2( safeZoneIndexX, safeZoneIndexY ) )].m_tileType = GRAVEL;
		}
	}
	for( int safeZoneIndexX = m_mapSize.x - 1; safeZoneIndexX > m_mapSize.x - 7; safeZoneIndexX-- )
	{
		for( int safeZoneIndexY = m_mapSize.y - 1; safeZoneIndexY > m_mapSize.y - 7; safeZoneIndexY-- )
		{
			m_tiles[GetTileIndexForTileCoordinates( IntVec2( safeZoneIndexX, safeZoneIndexY ) )].m_tileType = GRAVEL;
		}
	}





	//Add exit Tile
	m_tiles[GetTileIndexForTileCoordinates( IntVec2( m_mapSize.x - 3, m_mapSize.y - 3 ) )].m_tileType = EXIT;




	//Border wall
	int tileIndex = 0;
	for( int borderXIndex= 0; borderXIndex< m_mapSize.x; borderXIndex++ )
	{
		m_tiles[borderXIndex].m_tileType = BRICK;

		tileIndex = m_mapSize.x * (m_mapSize.y - 1) + borderXIndex;
		m_tiles[tileIndex].m_tileType = BRICK;
	}
	for( int borderYIndex= 0; borderYIndex < m_mapSize.y; borderYIndex++ )
	{
		tileIndex = borderYIndex * m_mapSize.x;
		m_tiles[tileIndex].m_tileType = BRICK;

		tileIndex = borderYIndex * m_mapSize.x + (m_mapSize.x-1);
		m_tiles[tileIndex].m_tileType = BRICK;
	}

	IntVec2 startPosition = Vec2( 2, 2 );
	IntVec2 exitPosition = IntVec2( m_mapSize.x - 3, m_mapSize.y - 3 );

	if( !ValidateTileMap( startPosition, exitPosition ) )
	{
		RebuildMap();
	}
}

void Map::RebuildLevelTwo()
{
	for( int tileIndex = 0; tileIndex < m_tiles.size(); tileIndex++ )
	{
		m_tiles[tileIndex].m_tileType = GRASS;
		m_tiles[tileIndex].m_validTile = -1;
	}



	int randomWormLength = 0;
	for( int wormIndex = 0; wormIndex < WORMCOUNT; wormIndex++ )
	{
		randomWormLength = m_game->m_rand.RollRandomIntInRange( 0, MAXWORMLENGTH );
		SpawnWorm( MUD, randomWormLength );
	}
	for( int wormIndex = 0; wormIndex < WORMCOUNT; wormIndex++ )
	{
		randomWormLength = m_game->m_rand.RollRandomIntInRange( 0, MAXWORMLENGTH );
		SpawnWorm( STONE, randomWormLength );
	}


	//Create Safe zone
	for( int safeZoneIndexX = 1; safeZoneIndexX < 6; safeZoneIndexX++ )
	{
		for( int safeZoneIndexY = 1; safeZoneIndexY < 6; safeZoneIndexY++ )
		{
			m_tiles[GetTileIndexForTileCoordinates( IntVec2( safeZoneIndexX, safeZoneIndexY ) )].m_tileType = GRASS;
		}
	}
	for( int safeZoneIndexX = m_mapSize.x - 1; safeZoneIndexX > m_mapSize.x - 7; safeZoneIndexX-- )
	{
		for( int safeZoneIndexY = m_mapSize.y - 1; safeZoneIndexY > m_mapSize.y - 7; safeZoneIndexY-- )
		{
			m_tiles[GetTileIndexForTileCoordinates( IntVec2( safeZoneIndexX, safeZoneIndexY ) )].m_tileType = GRASS;
		}
	}


	//Add exit Tile
	m_tiles[GetTileIndexForTileCoordinates( IntVec2( m_mapSize.x - 3, m_mapSize.y - 3 ) )].m_tileType = EXIT;




	//Border wall
	int tileIndex = 0;
	for( int borderXIndex= 0; borderXIndex< m_mapSize.x; borderXIndex++ )
	{
		m_tiles[borderXIndex].m_tileType = STONE;

		tileIndex = m_mapSize.x * (m_mapSize.y - 1) + borderXIndex;
		m_tiles[tileIndex].m_tileType = STONE;
	}
	for( int borderYIndex= 0; borderYIndex < m_mapSize.y; borderYIndex++ )
	{
		tileIndex = borderYIndex * m_mapSize.x;
		m_tiles[tileIndex].m_tileType = STONE;

		tileIndex = borderYIndex * m_mapSize.x + (m_mapSize.x-1);
		m_tiles[tileIndex].m_tileType = STONE;
	}

	IntVec2 startPosition = Vec2( 2, 2 );
	IntVec2 exitPosition = IntVec2( m_mapSize.x - 3, m_mapSize.y - 3 );

	if( !ValidateTileMap( startPosition, exitPosition ) )
	{
		RebuildMap();
	}
}

void Map::RebuildLevelThree()
{
	for( int tileIndex = 0; tileIndex < m_tiles.size(); tileIndex++ )
	{
		m_tiles[tileIndex].m_tileType = SAND;
		m_tiles[tileIndex].m_validTile = -1;
	}



	int randomWormLength = 0;
	for( int wormIndex = 0; wormIndex < WORMCOUNT; wormIndex++ )
	{
		randomWormLength = m_game->m_rand.RollRandomIntInRange( 0, MAXWORMLENGTH );
		SpawnWorm( LAVA, randomWormLength );
	}
	for( int wormIndex = 0; wormIndex < WORMCOUNT; wormIndex++ )
	{
		randomWormLength = m_game->m_rand.RollRandomIntInRange( 0, MAXWORMLENGTH );
		SpawnWorm( VOLCANICROCK, randomWormLength );
	}


	//Create Safe zone
	for( int safeZoneIndexX = 1; safeZoneIndexX < 6; safeZoneIndexX++ )
	{
		for( int safeZoneIndexY = 1; safeZoneIndexY < 6; safeZoneIndexY++ )
		{
			m_tiles[GetTileIndexForTileCoordinates( IntVec2( safeZoneIndexX, safeZoneIndexY ) )].m_tileType = SAND;
		}
	}
	for( int safeZoneIndexX = m_mapSize.x - 1; safeZoneIndexX > m_mapSize.x - 7; safeZoneIndexX-- )
	{
		for( int safeZoneIndexY = m_mapSize.y - 1; safeZoneIndexY > m_mapSize.y - 7; safeZoneIndexY-- )
		{
			m_tiles[GetTileIndexForTileCoordinates( IntVec2( safeZoneIndexX, safeZoneIndexY ) )].m_tileType = SAND;
		}
	}





	//Add exit Tile
	m_tiles[GetTileIndexForTileCoordinates( IntVec2( m_mapSize.x - 3, m_mapSize.y - 3 ) )].m_tileType = EXIT;




	//Border wall
	int tileIndex = 0;
	for( int borderXIndex= 0; borderXIndex< m_mapSize.x; borderXIndex++ )
	{
		m_tiles[borderXIndex].m_tileType = VOLCANICROCK;

		tileIndex = m_mapSize.x * (m_mapSize.y - 1) + borderXIndex;
		m_tiles[tileIndex].m_tileType = VOLCANICROCK;
	}
	for( int borderYIndex= 0; borderYIndex < m_mapSize.y; borderYIndex++ )
	{
		tileIndex = borderYIndex * m_mapSize.x;
		m_tiles[tileIndex].m_tileType = VOLCANICROCK;

		tileIndex = borderYIndex * m_mapSize.x + (m_mapSize.x-1);
		m_tiles[tileIndex].m_tileType = VOLCANICROCK;
	}

	IntVec2 startPosition = Vec2( 2, 2 );
	IntVec2 exitPosition = IntVec2( m_mapSize.x - 3, m_mapSize.y - 3 );

	if( !ValidateTileMap( startPosition, exitPosition ) )
	{
		RebuildMap();
	}
}

void Map::SpawnLevelOneEntities()
{

	for( int turretIndex = 0; turretIndex < LEVEL_ONE_TURRET_COUNT; turretIndex++ )
	{
		const Vec2 turretPos = GetRandomValidEntityPosition();
		const float randomOrientation = m_game->m_rand.RollRandomFloatInRange(0.f,359.9f);
		SpawnNewEntity(ENTITY_TYPE_NPC_TURRET, FACTION_EVIL, turretPos, randomOrientation);
	}

	for( int turretIndex = 0; turretIndex < LEVEL_ONE_TANK_COUNT; turretIndex++ )
	{
		const Vec2 turretPos = GetRandomValidEntityPosition();
		const float randomOrientation = m_game->m_rand.RollRandomFloatInRange( 0.f, 359.9f );
		SpawnNewEntity( ENTITY_TYPE_NPC_TANK, FACTION_EVIL, turretPos, randomOrientation );
	}

	for( int boulderIndex = 0; boulderIndex < LEVEL_ONE_BOULDER_COUNT; boulderIndex++ )
	{
		const Vec2 turretPos = GetRandomValidEntityPosition();
		const float randomOrientation = m_game->m_rand.RollRandomFloatInRange( 0.f, 359.9f );
		SpawnNewEntity( ENTITY_TYPE_BOULDER, FACTION_NEUTRAL, turretPos, randomOrientation );
	}

	SpawnNewEntity( ENTITY_TYPE_PLAYER, FACTION_GOOD, Vec2( 2.5f, 2.5f ), 0.f );
}

void Map::SpawnLevelTwoEntities()
{
	for( int turretIndex = 0; turretIndex < LEVEL_ONE_TURRET_COUNT; turretIndex++ )
	{
		const Vec2 turretPos = GetRandomValidEntityPosition();
		const float randomOrientation = m_game->m_rand.RollRandomFloatInRange( 0.f, 359.9f );
		SpawnNewEntity( ENTITY_TYPE_NPC_TURRET, FACTION_EVIL, turretPos, randomOrientation );
	}

	for( int turretIndex = 0; turretIndex < LEVEL_ONE_TANK_COUNT; turretIndex++ )
	{
		const Vec2 turretPos = GetRandomValidEntityPosition();
		const float randomOrientation = m_game->m_rand.RollRandomFloatInRange( 0.f, 359.9f );
		SpawnNewEntity( ENTITY_TYPE_NPC_TANK, FACTION_EVIL, turretPos, randomOrientation );
	}

	for( int boulderIndex = 0; boulderIndex < LEVEL_ONE_BOULDER_COUNT; boulderIndex++ )
	{
		const Vec2 turretPos = GetRandomValidEntityPosition();
		const float randomOrientation = m_game->m_rand.RollRandomFloatInRange( 0.f, 359.9f );
		SpawnNewEntity( ENTITY_TYPE_BOULDER, FACTION_NEUTRAL, turretPos, randomOrientation );
	}

	SpawnNewEntity( ENTITY_TYPE_PLAYER, FACTION_GOOD, Vec2( 2.5f, 2.5f ), 0.f );
}

void Map::SpawnLevelThreeEntities()
{
	for( int turretIndex = 0; turretIndex < LEVEL_ONE_TURRET_COUNT; turretIndex++ )
	{
		const Vec2 turretPos = GetRandomValidEntityPosition();
		const float randomOrientation = m_game->m_rand.RollRandomFloatInRange( 0.f, 359.9f );
		SpawnNewEntity( ENTITY_TYPE_NPC_TURRET, FACTION_EVIL, turretPos, randomOrientation );
	}

	for( int turretIndex = 0; turretIndex < LEVEL_ONE_TANK_COUNT; turretIndex++ )
	{
		const Vec2 turretPos = GetRandomValidEntityPosition();
		const float randomOrientation = m_game->m_rand.RollRandomFloatInRange( 0.f, 359.9f );
		SpawnNewEntity( ENTITY_TYPE_NPC_TANK, FACTION_EVIL, turretPos, randomOrientation );
	}

	for( int boulderIndex = 0; boulderIndex < LEVEL_ONE_BOULDER_COUNT; boulderIndex++ )
	{
		const Vec2 turretPos = GetRandomValidEntityPosition();
		const float randomOrientation = m_game->m_rand.RollRandomFloatInRange( 0.f, 359.9f );
		SpawnNewEntity( ENTITY_TYPE_BOULDER, FACTION_NEUTRAL, turretPos, randomOrientation );
	}

	SpawnNewEntity( ENTITY_TYPE_PLAYER, FACTION_GOOD, Vec2( 2.5f, 2.5f ), 0.f );
}

const Vec2 Map::GetRandomValidEntityPosition()
{
	IntVec2 validRandomTilePosition;
	validRandomTilePosition.x = -1;
	validRandomTilePosition.y = -1;

	while( !IsNonSolidTileCoordinates(validRandomTilePosition))
	{
		validRandomTilePosition.x = m_game->m_rand.RollRandomIntInRange(6,m_mapSize.x - 8);  //Avoiding Safe zones
		validRandomTilePosition.y = m_game->m_rand.RollRandomIntInRange(6,m_mapSize.y - 8); 
	}

	float randomX = m_game->m_rand.RollRandomFloatInRange(0.01f,0.99f);
	float randomY = m_game->m_rand.RollRandomFloatInRange(0.01f,0.99f);

	Vec2 validRandomEntityPosition = Vec2(validRandomTilePosition) + Vec2(randomX, randomY);		//Center of Tile
	return validRandomEntityPosition;
}

bool Map::IsNonSolidTileCoordinates( const IntVec2& tilePosition ) const
{
	if( IsValidTileCoordinates( tilePosition ) )
	{
		int tileIndex = GetTileIndexForTileCoordinates(tilePosition);
		if( IsTileSolid( tileIndex ) )
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		return false;
	}
}

void Map::CheckInput()
{
	const XboxController& controller = g_theInput->GetXboxController( 0 );
	if( controller.IsConnected() )
	{
		if( controller.GetButtonState( XBOX_BUTTON_ID_A ).WasJustPressed() && !GetPlayer()->IsGarbage())
		{
			SoundID playerShootSound = g_theAudio->CreateOrGetSound( "Data/Audio/PlayerShootNormal.ogg" );
			g_theAudio->PlaySound(playerShootSound);
			SpawnNewEntity(ENTITY_TYPE_GOOD_BULLET, FACTION_GOOD, GetPlayer()->GetNosePosition(), GetPlayer()->GetCannonOrientation());
		}

		if( controller.GetButtonState( XBOX_BUTTON_ID_B ).WasJustPressed() && GetPlayer()->IsGarbage() && m_PlayerLives > 0 )
		{
			m_PlayerLives--;
			m_EntityListsByType[ENTITY_TYPE_PLAYER][0] = new Player(Vec2(2.5f,2.5f), ENTITY_TYPE_PLAYER, FACTION_GOOD, m_game);
		}
	}
}


void Map::AddEntityToMap( Entity* newEntity )
{
	EntityList& myList = m_EntityListsByType[newEntity->m_EntityType];

	AddEntityToList(newEntity, myList);
}


void Map::AddEntityToList( Entity* newEntity, EntityList& myList )
{
	myList.push_back( newEntity );
}


void Map::UpdateEntitiesByList( EntityList& listToUpdate, float deltaSeconds )
{
	for( int entityIndex = 0; entityIndex < listToUpdate.size(); entityIndex++ )
	{
		if( listToUpdate[entityIndex] != nullptr )
		{
			listToUpdate[entityIndex]->Update(deltaSeconds);
		}

	}

}


void Map::RenderEntitiesByList( EntityList& listToRender )
{
	for( int entityIndex = 0; entityIndex < listToRender.size(); entityIndex++ )
	{
		if( listToRender[entityIndex] != nullptr )
		{
			listToRender[entityIndex]->Render();
		}
	}
}

void Map::ResolveEntityCollisionByList( EntityList& listToResolve )
{
	for( int entityIndex = 0; entityIndex < listToResolve.size(); entityIndex++ )
	{
		if( listToResolve[entityIndex] != nullptr )
		{
			ResolveEntityCollision(listToResolve[entityIndex]);
		}

	}
}

void Map::ResolveEntityCollision( Entity* entity )
{



	ResolveEntityEntityCollision( entity );
	ResolveEntityWallCollision( entity );


}



void Map::ResolveEntityWallCollision( Entity* entity )
{
	//Check if necessary to push entity
	if( entity->m_EntityType != ENTITY_TYPE_EVIL_BULLET
		&& entity->m_EntityType != ENTITY_TYPE_GOOD_BULLET
		&& !entity->IsPushedByWalls() )
	{
		return;
	}



	const IntVec2 entityPostionInt( entity->GetPosition() );
	Vec2 entityPosition( entity->GetPosition() );


	static const IntVec2 displacement9[] =
	{
		IntVec2( 0, 0 ),	//self
		IntVec2( -1, 0 ),	//left
		IntVec2( 1, 0 ),	//right
		IntVec2( 0, 1 ),	//up
		IntVec2( 0, -1 ),	//down
		IntVec2( -1, 1 ),	//top left
		IntVec2( 1, 1 ),	//top right
		IntVec2( -1, -1 ),	//bottom left
		IntVec2( 1, -1 ),	//bottom right
	};

	for( int positionIndex = 0; positionIndex < 9; positionIndex++ )
	{
		const IntVec2 currentPosition = entityPostionInt + displacement9[positionIndex];
		int currentTileIndex = GetTileIndexForTileCoordinates(currentPosition);
		if( currentTileIndex == -1 )
		{
			if( entity->m_EntityType == ENTITY_TYPE_GOOD_BULLET || entity->m_EntityType == ENTITY_TYPE_EVIL_BULLET )
			{
				//SpawnNewEntity(ENTITY_TYPE_EXPLOSION, FACTION_NEUTRAL, entityPosition, m_game->m_rand.GetRandomFloatInRange(0.f,359.f));
			}
			entity->MarkGarbage();
			break;
		}
		int tileType = m_tiles[currentTileIndex].m_tileType;
		
		if( TileDefinition::s_definitions[tileType].IsSolid() )
		{
			AABB2 tileAABB2 = m_tiles[currentTileIndex].GetAABB2();
			if( entity->m_EntityType == ENTITY_TYPE_GOOD_BULLET || entity->m_EntityType == ENTITY_TYPE_EVIL_BULLET )
			{
				if( DoDiscAndAABBOverlap2D( entityPosition, entity->m_physicsRadius, tileAABB2 ) )
				{
					entity->MarkGarbage();
					//SpawnNewEntity(ENTITY_TYPE_EXPLOSION, FACTION_NEUTRAL, entityPosition, m_game->m_rand.GetRandomFloatInRange(0.f,359.f));
				}
			}
			PushDiscOutOfAABB2D( entityPosition, entity->m_physicsRadius, tileAABB2 );
		}
	}

	entity->SetPosition( entityPosition );
}

void Map::ResolveEntityEntityCollision( Entity* entity )
{
	for( int entityListIndex = 0; entityListIndex < NUM_OF_ENTITY_TYPES; entityListIndex++ )
	{
		for( int entityIndex = 0; entityIndex < m_EntityListsByType[entityListIndex].size(); entityIndex++ )
		{
			Entity* otherEntity = m_EntityListsByType[entityListIndex][entityIndex];
			if( entity !=  otherEntity && !otherEntity->IsGarbage())
			{
				ResolveSingleEntityCollision(entity, otherEntity);
			}
		}
	}
}

void Map::CheckIfPlayerVisible()
{
	EntityList& npcTurretList = m_EntityListsByType[ENTITY_TYPE_NPC_TURRET];

	for( int npcTurretIndex = 0; npcTurretIndex < npcTurretList.size(); npcTurretIndex++ )
	{
		if( !npcTurretList[npcTurretIndex]->IsGarbage() )
		{
			if( HasLineOfSight( npcTurretList[npcTurretIndex] ) )
			{
				
				static_cast<NpcTurret*>(npcTurretList[npcTurretIndex])->m_DoesSeeEnemy = true;
				static_cast<NpcTurret*>(npcTurretList[npcTurretIndex])->m_HasSeenEnemy = true;
				static_cast<NpcTurret*>(npcTurretList[npcTurretIndex])->m_EnemyPosition = GetPlayer()->GetPosition();
			}
			else
			{
				static_cast<NpcTurret*>(npcTurretList[npcTurretIndex])->m_DoesSeeEnemy = false;
			}
		}

	}




	EntityList& npcTankList = m_EntityListsByType[ENTITY_TYPE_NPC_TANK];

	for( int npcTankIndex = 0; npcTankIndex < npcTankList.size(); npcTankIndex++ )
	{
		if( !npcTankList[npcTankIndex]->IsGarbage() )
		{
			NpcTank* npcTank = static_cast<NpcTank*>(npcTankList[npcTankIndex]);
			if( HasLineOfSight( npcTankList[npcTankIndex] ) )
			{

				npcTank->m_DoesSeeEnemy = true;
				npcTank->m_currentState = PURSUING;
				npcTank->m_EnemyPosition = GetPlayer()->GetPosition();

			}
			else
			{
				if( npcTank->m_currentState == PURSUING || npcTank->m_currentState == FINDING )
				{
					npcTank->m_currentState = FINDING;
				}
				else
				{
					npcTank->m_currentState = WANDERING;
				}
				npcTank->m_DoesSeeEnemy = false;

			}
		}

	}
}

bool Map::HasLineOfSight( Entity* entity )
{

	Player* player = static_cast<Player*>(m_EntityListsByType[ENTITY_TYPE_PLAYER][0]);
	RaycastResult results;
	if( entity->IsGarbage() || player->IsGarbage())
	{
		return false;
	}

	if( entity->m_EntityType == ENTITY_TYPE_NPC_TURRET )
	{
		
		if( GetDistance2D( entity->GetPosition(), player->GetPosition() ) > NPCTURRET_MAXRANGE )
		{
			return false;
		}
		else
		{
			results = RayCast(entity->GetPosition(), player->GetPosition());
		}
	}
	else if( entity->m_EntityType == ENTITY_TYPE_NPC_TANK )
	{
		if( GetDistance2D( entity->GetPosition(), player->GetPosition() ) > NPCTURRET_MAXRANGE )
		{
			return false;
		}
		else
		{
			results = RayCast(entity->GetPosition(), player->GetPosition());
		}
	}
	else
	{
		return false;
	}
	return results.m_didImpact;

}

void Map::RenderRaycasts()
{
	for( int rayCastIndex = 0; rayCastIndex < m_raycastResults.size(); rayCastIndex++ )
	{
		Vec2 startPos(m_raycastResults[rayCastIndex].m_startPos);
		Vec2 endPos(m_raycastResults[rayCastIndex].m_impactPos);

		g_theRenderer->BindTexture(nullptr);
		DrawLine(startPos, endPos, Rgba8(255,0,0), 0.01f);
	}
}

void Map::CreateRaycasts( EntityList& listToUpdate )
{
	RaycastResult result;
	for( int entityIndex = 0; entityIndex < listToUpdate.size(); entityIndex++ )
	{
		if( !listToUpdate[entityIndex]->IsGarbage() )
		{
			float entityOrientationDegrees = listToUpdate[entityIndex]->GetOrientationDegrees();
			Vec2 startPos = listToUpdate[entityIndex]->GetPosition();
			Vec2 endPos = Vec2::MakeFromPolarDegrees( entityOrientationDegrees, NPCTURRET_MAXRANGE ) + startPos;
			NpcTurret* npcTurret = static_cast<NpcTurret*>(listToUpdate[entityIndex]);

			result = RayCast( npcTurret->GetNosePosition(), endPos );
			
			m_raycastResults.push_back( result );
		}

	}
}

void Map::UpdateEntityStates( EntityList& listToUpdate, EntityType entityType )
{
	CheckIfPlayerVisible();
	Player* player = GetPlayer();

	if( player->IsGarbage() )
	{
		return;
	}
	const Vec2 playerPosition(player->GetPosition());

	if( entityType == ENTITY_TYPE_NPC_TANK )
	{
		for( int entityIndex = 0; entityIndex < listToUpdate.size(); entityIndex++ )
		{
			if( !listToUpdate[entityIndex]->IsGarbage() )
			{
				const Vec2 entityPosition( listToUpdate[entityIndex]->GetPosition() );
				const Vec2 entityFwdVector( listToUpdate[entityIndex]->GetForwardVector() );
				NpcTank* npcTank = static_cast<NpcTank*>(listToUpdate[entityIndex]);

				//Raycast for TankWhiskers

				const Vec2 leftWhiskerStartPos = entityFwdVector.GetRotated90Degrees()*NPCTANK_PHYSICS_RADIUS + entityPosition;
				const Vec2 rightWhiskeStartPos = entityFwdVector.GetRotatedMinus90Degrees()*NPCTANK_PHYSICS_RADIUS + entityPosition;
				const Vec2 middleWhiskerStartPos = entityPosition;
				const Vec2 leftWhiskerEndPos = entityFwdVector*NPCTANK_WHISKERRANGE + leftWhiskerStartPos;
				const Vec2 rightWhiskerEndPos = entityFwdVector*NPCTANK_WHISKERRANGE + rightWhiskeStartPos;
				const Vec2 middleWhiskerEndPos = entityFwdVector*NPC_TANK_MIDDLEWHISKERRANGE + middleWhiskerStartPos;

				RaycastResult leftWhiskerResults = RayCast(leftWhiskerStartPos, leftWhiskerEndPos);
				RaycastResult rightWhiskerResults = RayCast(rightWhiskeStartPos, rightWhiskerEndPos);
				RaycastResult middleWhiskerResults = RayCast(middleWhiskerStartPos, middleWhiskerEndPos);
				npcTank->m_leftWhisker = leftWhiskerResults.m_impactDist;
				npcTank->m_rightWhisker = rightWhiskerResults.m_impactDist;
				npcTank->m_middleWhisker = middleWhiskerResults.m_impactDist;



				if( IsPointInSector( playerPosition, entityPosition, NPCTANK_MAXRANGE, entityFwdVector, 90.f ) )
				{
					npcTank->m_CanDriveTowardsEnemy = true;

					if( IsPointInSector( playerPosition, entityPosition, NPCTANK_MAXRANGE, entityFwdVector, 5.f ) )
					{
						npcTank->m_CanShootEnemy = true;

						static float timer = (float)GetCurrentTimeSeconds();
						float currentTime = (float)GetCurrentTimeSeconds();
						if( currentTime > timer+1.8f )
						{
							SoundID enemyShootSound = g_theAudio->CreateOrGetSound( "Data/Audio/EnemyShoot.wav" );
							g_theAudio->PlaySound( enemyShootSound );
							SpawnNewEntity( ENTITY_TYPE_EVIL_BULLET, FACTION_EVIL, npcTank->GetNosePosition(), npcTank->GetOrientationDegrees() );

							timer = currentTime;
						}
					}
					else
					{
						npcTank->m_CanShootEnemy = false;
					}
				}
				else
				{
					npcTank->m_CanDriveTowardsEnemy = false;
				}
			}

		}
	}
	else if( entityType == ENTITY_TYPE_NPC_TURRET )
	{
		for( int entityIndex = 0; entityIndex < listToUpdate.size(); entityIndex++ )
		{
			if( !listToUpdate[entityIndex]->IsGarbage() )
			{
				const Vec2 entityPosition( listToUpdate[entityIndex]->GetPosition() );
				const Vec2 entityFwdVector( listToUpdate[entityIndex]->GetForwardVector() );
				NpcTurret* npcTurret = static_cast<NpcTurret*>(listToUpdate[entityIndex]);

				if( IsPointInSector( playerPosition, entityPosition, NPCTURRET_MAXRANGE, entityFwdVector, 5.f ) )
				{
					npcTurret->m_CanShootEnemy = true;

					static float timer = (float)GetCurrentTimeSeconds();
					float currentTime = (float)GetCurrentTimeSeconds();
					if( currentTime > timer+1.3f )
					{
						SoundID enemyShootSound = g_theAudio->CreateOrGetSound( "Data/Audio/EnemyShoot.wav" );
						g_theAudio->PlaySound( enemyShootSound );
						SpawnNewEntity( ENTITY_TYPE_EVIL_BULLET, FACTION_EVIL, npcTurret->GetNosePosition(), npcTurret->GetOrientationDegrees() );

						timer = currentTime;
					}
				}
				else
				{
					npcTurret->m_CanShootEnemy = false;
				}
			}

		}
	}

}

void Map::RenderTiles()
{
	m_vertsToRender.clear();
	for( int tileIndex = 0; tileIndex < m_tiles.size(); tileIndex++ )
	{
		m_tiles[tileIndex].AppendVerts( m_vertsToRender );
	}

	g_theRenderer->BindTexture(m_tileTexture);
	g_theRenderer->DrawVertexArray(m_vertsToRender);

}

void Map::RenderEntities()
{
	//Everything after ENTITY_TYPE_EXPLOSION will blend Additively

	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	for( int entityListIndex = 0; entityListIndex < NUM_OF_ENTITY_TYPES; entityListIndex++ )
	{
		if( entityListIndex == ENTITY_TYPE_EXPLOSION )
		{
			g_theRenderer->SetBlendMode(BlendMode::ADDITIVE);
		}
		EntityList& listToRender = m_EntityListsByType[entityListIndex];
		RenderEntitiesByList( listToRender );
	}


}

void Map::UpdateTiles( float deltaSeconds )
{
	UNUSED(deltaSeconds);
}


void Map::UpdateEntities( float deltaSeconds )
{
	m_raycastResults.clear();

	for( int entityListIndex = 0; entityListIndex < NUM_OF_ENTITY_TYPES; entityListIndex++ )
	{
		EntityList& listToUpdate = m_EntityListsByType[entityListIndex];
		UpdateEntitiesByList(listToUpdate, deltaSeconds);
		if( entityListIndex == ENTITY_TYPE_NPC_TURRET )
		{
			CreateRaycasts(listToUpdate);
			UpdateEntityStates(listToUpdate, ENTITY_TYPE_NPC_TURRET);
		}
		if( entityListIndex == ENTITY_TYPE_NPC_TANK )
		{
			UpdateEntityStates(listToUpdate, ENTITY_TYPE_NPC_TANK);
		}
	}






// 	for( int entityIndex = 0; entityIndex < m_entities.size(); entityIndex++ )
// 	{
// 		m_entities[entityIndex]->Update(deltaSeconds);
// 	}


	if( !g_theApp->IsNoClipping() )
	{
		for( int entityListIndex = 0; entityListIndex < NUM_OF_ENTITY_TYPES; entityListIndex++ )
		{
			EntityList& listToUpdate = m_EntityListsByType[entityListIndex];
			ResolveEntityCollisionByList( listToUpdate );
		}

		//PushPlayerOutOfWalls();
	}


}

void Map::SpawnTiles()
{

	switch( m_level )
	{
	case LEVELONE: SpawnLevelOneTiles();
		break;
	case LEVELTWO: SpawnLevelTwoTiles();
		break;
	case LEVELTHREE: SpawnLevelThreeTiles();
		break;
	default:
		break;
	}


}

void Map::SpawnEntities()
{
	switch( m_level )
	{
	case LEVELONE: SpawnLevelOneEntities();
		break;
	case LEVELTWO: SpawnLevelTwoEntities();
		break;
	case LEVELTHREE: SpawnLevelThreeEntities();
		break;
	default:
		break;
	}



}


void Map::PushPlayerOutOfWalls()
{
	//IntVec2 playerPostionInt(m_entities[0]->GetPosition());
	//Vec2 playerPosition(m_entities[0]->GetPosition());

	IntVec2 playerPostionInt( m_EntityListsByType[ENTITY_TYPE_PLAYER][0]->GetPosition() );
	Vec2 playerPosition( m_EntityListsByType[ENTITY_TYPE_PLAYER][0]->GetPosition() );

	//Grab all of the tile coordinates for tiles around the player
	IntVec2 leftPlayerPositionInt(playerPostionInt.x -1,playerPostionInt.y);
	IntVec2 rightPlayerPositionInt(playerPostionInt.x + 1, playerPostionInt.y);
	IntVec2 upPlayerPositionInt(playerPostionInt.x, playerPostionInt.y + 1);
	IntVec2 downPlayerPositionInt(playerPostionInt.x, playerPostionInt.y - 1);

	IntVec2 topLeftPlayerPositionInt( playerPostionInt.x -1, playerPostionInt.y + 1 );
	IntVec2 topRightPlayerPositionInt( playerPostionInt.x + 1, playerPostionInt.y + 1 );
	IntVec2 botttomLeftPlayerPositionInt( playerPostionInt.x - 1, playerPostionInt.y - 1 );
	IntVec2 bottomRightPlayerPositionInt( playerPostionInt.x + 1, playerPostionInt.y - 1 );




	//Push the player out of each of the surrounding tiles
	if( m_tiles[GetTileIndexForTileCoordinates( leftPlayerPositionInt )].m_tileType == STONE )
	{
		AABB2 leftTileAABB2 = m_tiles[GetTileIndexForTileCoordinates( leftPlayerPositionInt )].GetAABB2();
		PushDiscOutOfAABB2D( playerPosition, PLAYER_PHYSICS_RADIUS, leftTileAABB2 );
	}

	if( m_tiles[GetTileIndexForTileCoordinates( rightPlayerPositionInt )].m_tileType == STONE )
	{
		AABB2 rightTileAABB2 = m_tiles[GetTileIndexForTileCoordinates( rightPlayerPositionInt )].GetAABB2();
		PushDiscOutOfAABB2D( playerPosition, PLAYER_PHYSICS_RADIUS, rightTileAABB2 );
	}

	if( m_tiles[GetTileIndexForTileCoordinates( upPlayerPositionInt )].m_tileType == STONE )
	{
		AABB2 upTileAABB2 = m_tiles[GetTileIndexForTileCoordinates( upPlayerPositionInt )].GetAABB2();
		PushDiscOutOfAABB2D( playerPosition, PLAYER_PHYSICS_RADIUS, upTileAABB2 );
	}

	if( m_tiles[GetTileIndexForTileCoordinates( downPlayerPositionInt )].m_tileType == STONE )
	{
		AABB2 downTileAABB2 = m_tiles[GetTileIndexForTileCoordinates( downPlayerPositionInt )].GetAABB2();
		PushDiscOutOfAABB2D( playerPosition, PLAYER_PHYSICS_RADIUS, downTileAABB2 );
	}


	if( m_tiles[GetTileIndexForTileCoordinates( topLeftPlayerPositionInt )].m_tileType == STONE )
	{
		AABB2 topLeftTileAABB2 = m_tiles[GetTileIndexForTileCoordinates( topLeftPlayerPositionInt )].GetAABB2();
		PushDiscOutOfAABB2D( playerPosition, PLAYER_PHYSICS_RADIUS, topLeftTileAABB2 );
	}

	if( m_tiles[GetTileIndexForTileCoordinates( topRightPlayerPositionInt )].m_tileType == STONE )
	{
		AABB2 topRightTileAABB2 = m_tiles[GetTileIndexForTileCoordinates( topRightPlayerPositionInt )].GetAABB2();
		PushDiscOutOfAABB2D( playerPosition, PLAYER_PHYSICS_RADIUS, topRightTileAABB2 );
	}

	if( m_tiles[GetTileIndexForTileCoordinates( botttomLeftPlayerPositionInt )].m_tileType == STONE )
	{
		AABB2 bottomLeftTileAABB2 = m_tiles[GetTileIndexForTileCoordinates( botttomLeftPlayerPositionInt )].GetAABB2();
		PushDiscOutOfAABB2D( playerPosition, PLAYER_PHYSICS_RADIUS, bottomLeftTileAABB2 );
	}

	if( m_tiles[GetTileIndexForTileCoordinates( bottomRightPlayerPositionInt )].m_tileType == STONE )
	{
		AABB2 bottomRightTileAABB2 = m_tiles[GetTileIndexForTileCoordinates( bottomRightPlayerPositionInt )].GetAABB2();
		PushDiscOutOfAABB2D( playerPosition, PLAYER_PHYSICS_RADIUS, bottomRightTileAABB2 );
	}
	//m_entities[0]->SetPosition(playerPosition);
	m_EntityListsByType[ENTITY_TYPE_PLAYER][0]->SetPosition(playerPosition);
}


void Map::GarbageCollectEntities()
{
	for( int entityListIndex = 0; entityListIndex < NUM_OF_ENTITY_TYPES; entityListIndex++ )
	{
		EntityList& listToGarbageCollect = m_EntityListsByType[entityListIndex];
		GarbageCollectEntitiesByList( listToGarbageCollect );
	}
}

void Map::GarbageCollectEntitiesByList( EntityList& listToGarbageCollect )
{
	for( int entityIndex = 0; entityIndex < listToGarbageCollect.size(); entityIndex++ )
	{
		if( listToGarbageCollect[entityIndex]->IsGarbage() && listToGarbageCollect[entityIndex] != nullptr)
		{
			Vec2 entityPosition = listToGarbageCollect[entityIndex]->GetPosition();
			EntityType entityType = listToGarbageCollect[entityIndex]->m_EntityType;
			switch( entityType )
			{
			case ENTITY_TYPE_PLAYER:		SpawnNewEntity(ENTITY_TYPE_EXPLOSION, FACTION_NEUTRAL, entityPosition, m_game->m_rand.RollRandomFloatInRange(0.f,359.f), EXPLOSION_SIZE * 5.f);
				break;
			case ENTITY_TYPE_NPC_TURRET:	SpawnNewEntity(ENTITY_TYPE_EXPLOSION, FACTION_NEUTRAL, entityPosition, m_game->m_rand.RollRandomFloatInRange(0.f,359.f), EXPLOSION_SIZE);
				break;
			case ENTITY_TYPE_NPC_TANK:		SpawnNewEntity(ENTITY_TYPE_EXPLOSION, FACTION_NEUTRAL, entityPosition, m_game->m_rand.RollRandomFloatInRange(0.f,359.f), EXPLOSION_SIZE);
				break;
			case ENTITY_TYPE_GOOD_BULLET:	SpawnNewEntity(ENTITY_TYPE_EXPLOSION, FACTION_NEUTRAL, entityPosition, m_game->m_rand.RollRandomFloatInRange(0.f,359.f), EXPLOSION_SIZE * 0.5f);
				break;
			case ENTITY_TYPE_EVIL_BULLET:	SpawnNewEntity(ENTITY_TYPE_EXPLOSION, FACTION_NEUTRAL, entityPosition, m_game->m_rand.RollRandomFloatInRange(0.f,359.f), EXPLOSION_SIZE * 0.5f);
				break;
			default:
				break;
			}
			delete listToGarbageCollect[entityIndex];
			listToGarbageCollect[entityIndex] = nullptr;
		}
		
	}
}

RaycastResult Map::RayCast( const Vec2 startPos, const Vec2 endPos )
{
	RaycastResult results;
	float startStep = 0.f;
	float distanceToEndPos = GetDistance2D(startPos, endPos);
	Vec2 displacementVector = endPos - startPos;
	float displacementAngle = displacementVector.GetAngleDegrees();
	Vec2 currentStepPosition = startPos;

	results.m_startPos = startPos;

	//int tileIndex = 0;
	while( startStep < distanceToEndPos )
	{
		currentStepPosition = Vec2::MakeFromPolarDegrees(displacementAngle, startStep) + startPos;
		const IntVec2 currentIntStepPosition(currentStepPosition);
		const int tileIndex = GetTileIndexForTileCoordinates(currentIntStepPosition);

		Tile currentTile = m_tiles[tileIndex];
		TileDefinition tileDef = TileDefinition::s_definitions[currentTile.m_tileType];
		results.m_impactTileType = currentTile.m_tileType;
		
		if( tileDef.IsSolid() )
		{
			break;
		}
		startStep += RAYCAST_STEP;


	}

	

	results.m_impactPos = Vec2::MakeFromPolarDegrees(displacementAngle, startStep);
	results.m_impactPos += startPos;

	results.m_impactDist = startStep;





	if( startStep > distanceToEndPos )
	{
		results.m_didImpact = true;
	}
	else
	{
		results.m_didImpact = false;
	}
	//results.m_impactDist = Clampf(results.m_impactDist, 0.f, distanceToEndPos);


	if( g_theApp->GetDebugGameMode() )
	{
		m_raycastResults.push_back(results);

	}

	return results;
}

bool Map::IsPointInSector( const Vec2& point, const Vec2& startPos, float maxDist, const Vec2& fwdDir, float apertureDegrees )
{
// 	Check distance
// 		FwdAngle = GetAngle for fwdDir
// 		pointAngle = GetDisplacement angle
// 		deltaAngle = GetShortestDisplacement( pointAngle, FwdAngle )
// 		If( deltaAngle < apertureDegrees*0.5

	float distance = GetDistance2D(point, startPos);
	if( distance > maxDist )
	{
		return false;
	}
	

	Vec2 displacementVec(point - startPos);
	float deltaDegrees = GetShortestAngularDisplacement(displacementVec.GetAngleDegrees(), fwdDir.GetAngleDegrees());
	deltaDegrees  = absFloat(deltaDegrees);

	if( deltaDegrees < apertureDegrees * 0.5f )
	{
		if( RayCast( startPos, point ).m_didImpact )
		{
			return true;
		}


	}

	return false;
}

bool Map::IsPointInSolid( const Vec2& point, Entity* entity /*= nullptr*/, AABB2* aabb /*= nullptr*/ )
{
	//Compare against either an entity or the aabb of a tile
	if( entity != nullptr )
	{ 

		return DoDiscsOverlap( point, 0.f, entity->GetPosition(), entity->m_physicsRadius );
	}
	else if( aabb != nullptr )
	{
		return aabb->IsPointInside( point );

	}
	else
	{
		return false;
	}
}

bool Map::IsTileSolid( int tileIndex ) const
{
	int tileType = m_tiles[tileIndex].m_tileType;
	return  TileDefinition::s_definitions[tileType].IsSolid();
}

void Map::SpawnWorm( TileType tileType, int wormLength )
{
	//Get Random Tile
	int randomX = m_game->m_rand.RollRandomIntInRange(1, m_mapSize.x - 2);
	int randomY = m_game->m_rand.RollRandomIntInRange(1, m_mapSize.y - 2);
	IntVec2 currentRandomTilePosition(randomX, randomY);
	int currentRandomValidTileIndex = GetTileIndexForTileCoordinates(currentRandomTilePosition);

	m_tiles[currentRandomValidTileIndex].m_tileType = tileType;


	for( int wormLengthIndex = 0; wormLengthIndex < wormLength - 1; wormLengthIndex++ )
	{

		currentRandomTilePosition = GetRandomValidAdjacentTilePosition(currentRandomTilePosition);
		currentRandomValidTileIndex = GetTileIndexForTileCoordinates(currentRandomTilePosition);
		
		m_tiles[currentRandomValidTileIndex].m_tileType = tileType;
	}

}

bool Map::IsValidTileCoordinates( const IntVec2& tileCoords ) const
{
	//Check if min bounds
	if( tileCoords.x < 0 || tileCoords.y < 0 )
	{
		return false;
	}
	//Check max bounds
	if( tileCoords.x >= m_mapSize.x || tileCoords.y >= m_mapSize.y )
	{
		return false;
	}

	return true;
}

int Map::FloodFill( IntVec2 startPosition, int currentStep, std::vector<int>& validMapTiles )
{
	if( currentStep < 0 )
	{
		return currentStep;
	}
	else
	{



		IntVec2 leftVec(-1,0);
		IntVec2 rightVec(1,0);
		IntVec2 upVec(0,1);
		IntVec2 downVec(0,-1);

		int leftTileIndex = GetTileIndexForTileCoordinates(startPosition + leftVec);
		int rightTileIndex = GetTileIndexForTileCoordinates( startPosition + rightVec );
		int upTileIndex = GetTileIndexForTileCoordinates( startPosition + upVec );
		int downTileIndex = GetTileIndexForTileCoordinates( startPosition + downVec );


		//Recursively check for valid left, right,up, and down tile coordinates

		if( IsNonSolidTileCoordinates( startPosition + leftVec ) )
		{

			if( validMapTiles[leftTileIndex] == -1 )
			{
				validMapTiles[leftTileIndex] = 1;
				m_tiles[leftTileIndex].m_validTile = 1;

				currentStep = FloodFill( startPosition + leftVec, currentStep - 1, validMapTiles );
				if( currentStep < 0 )
				{
					return currentStep;
				}
			}

		}
		else if( IsValidTileCoordinates( startPosition + leftVec ) )
		{
			validMapTiles[leftTileIndex] = 0;
			m_tiles[leftTileIndex].m_validTile = 0;
		}

		if( IsNonSolidTileCoordinates( startPosition + rightVec ) )
		{

			if( validMapTiles[rightTileIndex] == -1 )
			{
				validMapTiles[rightTileIndex] = 1;
				m_tiles[rightTileIndex].m_validTile = 1;

				currentStep = FloodFill( startPosition + rightVec, currentStep - 1, validMapTiles );
				if( currentStep < 0 )
				{
					return currentStep;
				}
			}
		}
		else if( IsValidTileCoordinates( startPosition + rightVec ) )
		{
			validMapTiles[rightTileIndex] = 0;
			m_tiles[rightTileIndex].m_validTile = 0;
		}

		if( IsNonSolidTileCoordinates( startPosition + upVec ) )
		{

			if( validMapTiles[upTileIndex] == -1 )
			{
				validMapTiles[upTileIndex] = 1;
				m_tiles[upTileIndex].m_validTile = 1;

				currentStep = FloodFill( startPosition + upVec, currentStep - 1, validMapTiles );
				if( currentStep < 0 )
				{
					return currentStep;
				}
			}
		}
		else if( IsValidTileCoordinates( startPosition + upVec ) )
		{
			validMapTiles[upTileIndex] = 0;
			m_tiles[upTileIndex].m_validTile = 0;
		}

		if( IsNonSolidTileCoordinates( startPosition + downVec ) )
		{

			if( validMapTiles[downTileIndex] == -1 )
			{
				validMapTiles[downTileIndex] = 1;
				m_tiles[downTileIndex].m_validTile = 1;

				currentStep = FloodFill( startPosition + downVec, currentStep - 1, validMapTiles );
				if( currentStep < 0 )
				{
					return currentStep;
				}
			}
		}
		else if( IsValidTileCoordinates( startPosition + downVec ) )
		{
			validMapTiles[downTileIndex] = 0;
			m_tiles[downTileIndex].m_validTile = 0;
		}

	}
	return currentStep;
}

void Map::ResolveSingleEntityCollision( Entity* entity, Entity* otherEntity )
{
	Vec2 entityPosition = entity->GetPosition();
	Vec2 otherEntityPosition = otherEntity->GetPosition();

	if( entity->DoesPushEntities() )
	{
		if( otherEntity->DoesPushEntities() )
		{
			if( entity->IsPushedByEntities() && otherEntity->IsPushedByEntities() )
			{
				PushDiscsOutOfEachOther2D(entityPosition, entity->m_physicsRadius, otherEntityPosition, otherEntity->m_physicsRadius);
			}
			else if( entity->IsPushedByEntities() && !otherEntity->IsPushedByEntities() )
			{
				PushDiscOutOfDisc2D(entityPosition, entity->m_physicsRadius, otherEntityPosition, otherEntity->m_physicsRadius);
			}
			else if( !entity->IsPushedByEntities() && otherEntity->IsPushedByEntities() )
			{
				PushDiscOutOfDisc2D(otherEntityPosition, otherEntity->m_physicsRadius, entityPosition, entity->m_physicsRadius );
			}
		}
		else
		{
			if( otherEntity->IsPushedByEntities() )
			{
				PushDiscOutOfDisc2D(otherEntityPosition, otherEntity->m_physicsRadius, entityPosition, entity->m_physicsRadius );
			}
		}
	}
	else if( otherEntity->DoesPushEntities() )
	{
		if( entity->IsPushedByEntities() )
		{
			PushDiscOutOfDisc2D(entityPosition, entity->m_physicsRadius, otherEntityPosition, otherEntity->m_physicsRadius);
		}
	}

	if( otherEntity->IsHitByBullets() )
	{
		if( otherEntity->m_EntityType == ENTITY_TYPE_BOULDER )
		{
			if( entity->m_EntityType == ENTITY_TYPE_GOOD_BULLET || entity->m_EntityType == ENTITY_TYPE_EVIL_BULLET )
			{
				if( DoDiscsOverlap( entityPosition, entity->m_physicsRadius, otherEntityPosition, otherEntity->m_physicsRadius ) )
				{
					Vec2 normalVec = entityPosition - otherEntityPosition;
					Vec2 BulletVelocity = entity->GetForwardVelocityVector();
					BulletVelocity.Reflect(normalVec);
					entity->SetVelocity(BulletVelocity);

					//SpawnNewEntity(ENTITY_TYPE_EXPLOSION, FACTION_NEUTRAL, entityPosition, m_game->m_rand.GetRandomFloatInRange(0.f,359.f));
				}
			}
		}
		else if( entity->m_EntityType == ENTITY_TYPE_GOOD_BULLET && otherEntity->m_EntityFaction != FACTION_GOOD)
		{
			if( DoDiscsOverlap( entityPosition, entity->m_physicsRadius, otherEntityPosition, otherEntity->m_physicsRadius ) )
			{
				entity->MarkGarbage();
				otherEntity->Lose1Health();
				//SpawnNewEntity(ENTITY_TYPE_EXPLOSION, FACTION_NEUTRAL, entityPosition, m_game->m_rand.GetRandomFloatInRange(0.f,359.f));
			}
		}
		else if( entity->m_EntityType == ENTITY_TYPE_EVIL_BULLET && otherEntity->m_EntityFaction != FACTION_EVIL )
		{
			if( DoDiscsOverlap( entityPosition, entity->m_physicsRadius, otherEntityPosition, otherEntity->m_physicsRadius ) )
			{
				entity->MarkGarbage();
				otherEntity->Lose1Health();
				//SpawnNewEntity(ENTITY_TYPE_EXPLOSION, FACTION_NEUTRAL, entityPosition, m_game->m_rand.GetRandomFloatInRange(0.f,359.f));
			}
		}
	}


	entity->SetPosition(entityPosition);
	otherEntity->SetPosition(otherEntityPosition);
}

int Map::GetTileIndexForTileCoordinates( const IntVec2& tileCoords ) const
{
	if( !IsValidTileCoordinates( tileCoords ) )
	{
		return -1;
	}
	int tileIndex = tileCoords.x + m_mapSize.x * tileCoords.y;
	return tileIndex;

}
IntVec2 Map::GetTileCoordinatesForTileIndex( int tileIndex ) const
{
	return m_tiles[tileIndex].m_tileCoords;

}

const TileDefinition& Map::GetTileDefinition() const
{
	return m_tileDefinition;
}

