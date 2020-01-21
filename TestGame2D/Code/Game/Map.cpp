#include "Game/TileMetaData.hpp"
#include "Game/Map.hpp"
#include "Game/Tile.hpp"
#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/App.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/Actor.hpp"
#include "Game/ActorDefinition.hpp"
#include "Game/MapGenStep.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/StringUtils.hpp"


extern App* g_theApp;
extern RenderContext* g_theRenderer;


Map::Map( const char* mapDefName )
{
	m_mapDef = MapDefinition::s_definitions[mapDefName];
	GUARANTEE_OR_DIE(m_mapDef, "Map definition doesn't exist");
	
	m_name = m_mapDef->m_name;
	m_mapSize = m_mapDef->m_mapDimensions;
	SpawnTiles();
	SpawnEntities();
}

Map::~Map()
{}


void Map::Startup()
{
	for( int tileIndex = 0; tileIndex < m_tiles.size(); tileIndex++ )
	{
		m_tiles[tileIndex].AppendVerts(m_vertsToRender);
	}
}


void Map::Shutdown()
{

}


void Map::Update( float deltaSeconds )
{
	UpdateTiles(deltaSeconds);
	UpdateEntities(deltaSeconds);
	UpdateDebugMode();
}

void Map::Render()
{
	RenderTiles();
	RenderEntities();

	if( g_theApp->GetDebugGameMode() )
	{
		//RenderDebugMode();
	}
}


Actor* Map::GetPlayer()
{
	return static_cast<Actor*>(m_entities[0]);
}


void Map::GetPlayers(std::vector<Actor*>& players)
{
	players.push_back(static_cast<Actor*>(m_entities[0]));
	players.push_back(static_cast<Actor*>(m_entities[1]));
	players.push_back(static_cast<Actor*>(m_entities[2]));
	players.push_back(static_cast<Actor*>(m_entities[3]));
}

IntVec2 Map::GetMapBounds() const
{
	return m_mapSize;
}


std::vector<Tile*> Map::GetNeighboringTiles( Tile* currentTile )
{
	std::vector<Tile*> neighboringTiles;

	Tile* northTile = GetTileAtPosition(currentTile->m_tileCoords + STEP_NORTH);
	Tile* westTile = GetTileAtPosition(currentTile->m_tileCoords + STEP_WEST);
	Tile* southTile = GetTileAtPosition(currentTile->m_tileCoords + STEP_SOUTH);
	Tile* eastTile = GetTileAtPosition(currentTile->m_tileCoords + STEP_EAST);
	Tile* northWestTile = GetTileAtPosition(currentTile->m_tileCoords + STEP_NORTHWEST);
	Tile* southWestTile = GetTileAtPosition(currentTile->m_tileCoords + STEP_SOUTHWEST);
	Tile* southEastTile = GetTileAtPosition(currentTile->m_tileCoords + STEP_SOUTHEAST);
	Tile* northEastTile = GetTileAtPosition(currentTile->m_tileCoords + STEP_NORTHEAST);

	if(northTile )		neighboringTiles.push_back(northTile); 
	if(westTile )		neighboringTiles.push_back(westTile); 
	if(southTile )		neighboringTiles.push_back(southTile); 
	if(eastTile )		neighboringTiles.push_back(eastTile); 
	if(northWestTile )	neighboringTiles.push_back(northWestTile); 
	if(southWestTile )	neighboringTiles.push_back(southWestTile); 
	if(southEastTile )	neighboringTiles.push_back(southEastTile); 
	if(northEastTile )	neighboringTiles.push_back(northEastTile); 

	return neighboringTiles;
}


void Map::RenderTiles()
{
	const Texture tex = g_tileSpriteSheet->GetTexture();
	g_theRenderer->BindTexture(&tex);
	g_theRenderer->DrawVertexArray(m_vertsToRender);
}


void Map::RenderEntities()
{
	for( int entityIndex = 0; entityIndex < m_entities.size(); entityIndex++ )
	{
		m_entities[entityIndex]->Render();
	}
}


void Map::UpdateTiles( float deltaSeconds )
{
	UNUSED(deltaSeconds);
}


void Map::UpdateEntities( float deltaSeconds )
{
	for( int entityIndex = 0; entityIndex < m_entities.size(); entityIndex++ )
	{
		m_entities[entityIndex]->Update(deltaSeconds);
	}

	if( !g_theApp->IsNoClipping() )
	{
		PushEntitiesOutOfWalls();
	}
}

Tile& Map::GetRandomMapTile()
{
	IntRange xMapRange(1, m_mapSize.x - 2);
	IntRange yMapRange(1, m_mapSize.y - 2);

	IntVec2 randomMapCoordinates;
	randomMapCoordinates.x = xMapRange.GetRandomInRange(g_theGame->m_rand);
	randomMapCoordinates.y = yMapRange.GetRandomInRange(g_theGame->m_rand);

	int tileIndex = GetTileIndexForTileCoordinates(randomMapCoordinates);

	Tile& randomTile = m_tiles[tileIndex];
	return randomTile;
}


Tile& Map::GetRandomAdjacentMapTile(Tile& currentTile)
{
	int randomDirection = g_theGame->m_rand.RollRandomIntInRange(0,3);
	IntVec2 currentTilePosition = currentTile.m_tileCoords;
	IntVec2 directionToChangeTilePosition(0,0);

	if( randomDirection == 0 )
	{
		directionToChangeTilePosition = IntVec2( 1, 0 );
	}
	else if( randomDirection == 1 )
	{
		directionToChangeTilePosition = IntVec2( 0, 1 );
	}
	else if( randomDirection == 2 )
	{
		directionToChangeTilePosition = IntVec2( -1, 0 );
	}
	else if( randomDirection == 3 )
	{
		directionToChangeTilePosition = IntVec2( 0, -1 );
	}

	while( !IsTilePositionValid( currentTilePosition+directionToChangeTilePosition ) )
	{
		randomDirection = g_theGame->m_rand.RollRandomIntInRange(0,3);
		if( randomDirection == 0 )
		{
			directionToChangeTilePosition = IntVec2(1,0);
		}
		else if( randomDirection == 1 )
		{
			directionToChangeTilePosition = IntVec2(0,1);
		}
		else if( randomDirection == 2 )
		{
			directionToChangeTilePosition = IntVec2(-1,0);
		}
		else if( randomDirection == 3 )
		{
			directionToChangeTilePosition = IntVec2(0,-1);
		}
	}
	IntVec2 newTilePosition = currentTilePosition + directionToChangeTilePosition;
	int newTileIndex = GetTileIndexForTileCoordinates(newTilePosition);
	Tile& newTile = m_tiles[newTileIndex];

	return newTile;
}


void Map::SpawnTiles()
{
	int x = 0;
	int y = 0;
	std::string fillTile = m_mapDef->m_fillTile;
	std::string edgeTile = m_mapDef->m_edgeTile;
	TileDefinition* fillTileDef = TileDefinition::s_definitions[fillTile.c_str()];
	TileDefinition* edgeTileDef = TileDefinition::s_definitions[edgeTile.c_str()];
	int numOfTiles = m_mapSize.x * m_mapSize.y;

	for( int mapIndex = 0; mapIndex < numOfTiles; mapIndex++ )
	{
		y = mapIndex / (m_mapSize.x);
		x = mapIndex - y * m_mapSize.x;

		m_tiles.push_back(Tile(IntVec2(x,y), fillTileDef));
		m_tileMetaData.push_back(TileMetaData());
	}

	//MapGenSteps
	std::vector<MapGenStep*>& mapGenSteps = m_mapDef->m_mapGenSteps;
	for( int mapGenStepIndex = 0; mapGenStepIndex < mapGenSteps.size(); mapGenStepIndex++ )
	{
		mapGenSteps[mapGenStepIndex]->RunStep(*this);
	}

// 	//Create Safe zone
// 	for( int safeZoneIndexX = 1; safeZoneIndexX < 6; safeZoneIndexX++ )
// 	{
// 		for( int safeZoneIndexY = 1; safeZoneIndexY < 6; safeZoneIndexY++ )
// 		{
// 			int tileIndex = GetTileIndexForTileCoordinates( IntVec2( safeZoneIndexX, safeZoneIndexY ) );
// 			m_tiles[tileIndex].SetTileType(fillTileDef);
// 		}
// 	}
// 	for( int safeZoneIndexX = m_mapSize.x - 1; safeZoneIndexX > m_mapSize.x - 7; safeZoneIndexX-- )
// 	{
// 		for( int safeZoneIndexY = m_mapSize.y - 1; safeZoneIndexY > m_mapSize.y - 7; safeZoneIndexY-- )
// 		{
// 			int tileIndex = GetTileIndexForTileCoordinates( IntVec2( safeZoneIndexX, safeZoneIndexY ) );
// 			m_tiles[tileIndex].SetTileType( fillTileDef );
// 		}
// 	}
// 
// 	//Border wall
// 	int tileIndex = 0;
// 	for( int borderXIndex= 0; borderXIndex < m_mapSize.x; borderXIndex++ )
// 	{
// 		m_tiles[borderXIndex].SetTileType(edgeTileDef);
// 
// 		tileIndex = m_mapSize.x * (m_mapSize.y - 1) + borderXIndex;
// 		m_tiles[tileIndex].SetTileType(edgeTileDef);
// 	}
// 	for( int borderYIndex= 0; borderYIndex < m_mapSize.y; borderYIndex++ )
// 	{
// 		tileIndex = borderYIndex * m_mapSize.x;
// 		m_tiles[tileIndex].SetTileType(edgeTileDef);
// 
// 		tileIndex = borderYIndex * m_mapSize.x + (m_mapSize.x-1);
// 		m_tiles[tileIndex].SetTileType(edgeTileDef);
// 	}
}

void Map::SpawnEntities()
{
	ActorDefinition* playerActorDef = ActorDefinition::s_definitions["Player"];

	ActorDefinition* maryActorDef = ActorDefinition::s_definitions["Mary"];
	ActorDefinition* josenActorDef = ActorDefinition::s_definitions["Josen"];

	m_entities.push_back( new Actor(Vec2(2.5, 2.5f),Vec2(0.f,0.f), 0.f, 0.f, playerActorDef, Player_1));
	m_entities.push_back( new Actor(Vec2(3.f, 3.f),Vec2(0.f,0.f), 0.f, 0.f, playerActorDef, Player_2));
	m_entities.push_back( new Actor(Vec2(2.f, 2.f),Vec2(0.f,0.f), 0.f, 0.f, playerActorDef, Player_3));
	m_entities.push_back( new Actor(Vec2(2.f, 2.5f),Vec2(0.f,0.f), 0.f, 0.f, playerActorDef, Player_4));
	//m_entities.push_back( new Actor(Vec2(3.5f, 3.5f),Vec2(0.f,0.f), 0.f, 0.f, maryActorDef));
	//m_entities.push_back( new Actor(Vec2(4.5f, 4.5f),Vec2(0.f,0.f), 0.f, 0.f, josenActorDef));
}


void Map::PushEntityOutOfWalls(Entity* currentEntity)
{
	IntVec2 entityPostionInt(currentEntity->GetPosition());
	Vec2 entityPosition(currentEntity->GetPosition());

	//Grab all of the tile coordinates for tiles around the player
	IntVec2 leftPlayerPositionInt(entityPostionInt.x - 1,entityPostionInt.y);
	IntVec2 rightPlayerPositionInt(entityPostionInt.x + 1, entityPostionInt.y);
	IntVec2 upPlayerPositionInt(entityPostionInt.x, entityPostionInt.y + 1);
	IntVec2 downPlayerPositionInt(entityPostionInt.x, entityPostionInt.y - 1);
	IntVec2 topLeftPlayerPositionInt( entityPostionInt.x - 1, entityPostionInt.y + 1 );
	IntVec2 topRightPlayerPositionInt( entityPostionInt.x + 1, entityPostionInt.y + 1 );
	IntVec2 botttomLeftPlayerPositionInt( entityPostionInt.x - 1, entityPostionInt.y - 1 );
	IntVec2 bottomRightPlayerPositionInt( entityPostionInt.x + 1, entityPostionInt.y - 1 );

	const Tile& leftTile		= m_tiles[GetTileIndexForTileCoordinates( leftPlayerPositionInt )];
	const Tile& rightTile		= m_tiles[GetTileIndexForTileCoordinates( rightPlayerPositionInt)];
	const Tile& upTile			= m_tiles[GetTileIndexForTileCoordinates( upPlayerPositionInt )];
	const Tile& downTile		= m_tiles[GetTileIndexForTileCoordinates( downPlayerPositionInt )];
	const Tile& topLeftTile		= m_tiles[GetTileIndexForTileCoordinates( topLeftPlayerPositionInt )];
	const Tile& topRightTile	= m_tiles[GetTileIndexForTileCoordinates( topRightPlayerPositionInt )];
	const Tile& bottomLeftTile	= m_tiles[GetTileIndexForTileCoordinates( botttomLeftPlayerPositionInt )];
	const Tile& bottomRightTile = m_tiles[GetTileIndexForTileCoordinates( bottomRightPlayerPositionInt )];

	PushEntityOutOfWall( currentEntity, leftTile);
	PushEntityOutOfWall( currentEntity, rightTile);
	PushEntityOutOfWall( currentEntity, upTile);
	PushEntityOutOfWall( currentEntity, downTile);
	PushEntityOutOfWall( currentEntity, topLeftTile);
	PushEntityOutOfWall( currentEntity, topRightTile);
	PushEntityOutOfWall( currentEntity, bottomLeftTile);
	PushEntityOutOfWall( currentEntity, bottomRightTile);
}


void Map::PushEntityOutOfWall( Entity* entityToPush, const Tile& tile )
{
	Vec2 entityPosition( entityToPush->GetPosition() );
	float physicsRadius = entityToPush->m_physicsRadius;
	AABB2 tileAABB2 = tile.GetAABB2();

	bool entityCanWalk = entityToPush->m_canWalk;
	bool entityCanFly = entityToPush->m_canFly;
	bool entityCanSwim = entityToPush->m_canSwim;

	bool tileAllowsWalking = tile.m_tileDef->m_allowsWalking;
	bool tileAllowsFlying = tile.m_tileDef->m_allowsFlying;
	bool TileAllowsSwimming = tile.m_tileDef->m_allowsSwimming;


	if( tileAllowsWalking && entityCanWalk )
	{
		return;
	}
	if( tileAllowsFlying && entityCanFly )
	{
		return;
	}
	if( TileAllowsSwimming && entityCanSwim )
	{
		return;
	}

	PushDiscOutOfAABB2D( entityPosition, physicsRadius, tileAABB2 );

	if( entityPosition != entityToPush->m_position )
	{
		entityToPush->m_hasCollided = true;
	}
	entityToPush->SetPosition(entityPosition);
}


void Map::PushEntitiesOutOfWalls()
{
	for( int entityIndex = 0; entityIndex < m_entities.size(); entityIndex++ )
	{
		PushEntityOutOfWalls(m_entities[entityIndex]);
	}
}


void Map::UpdateEntityMouseIsTouching()
{
	for( int entityIndex = 0; entityIndex < m_entities.size(); entityIndex++ )
	{
		SetMouseIsTouchingEntity( m_entities[entityIndex] );

		if( m_entityMouseIsTouching )
		{
			break;
		}
	}
}


void Map::UpdateTileMouseIsTouching()
{
	m_tileMouseIsTouching = nullptr;

	IntVec2 mouseTilePosition = IntVec2(g_theGame->GetMousePositionOnMainCamera());

	if( IsTilePositionValid( mouseTilePosition ) )
	{
		int tileIndex = GetTileIndexForTileCoordinates( mouseTilePosition );

		if( tileIndex )
		{
			m_tileMouseIsTouching = &(m_tiles[tileIndex]);
		}
	}
}


void Map::SetMouseIsTouchingEntity( Entity* entity )
{
	m_entityMouseIsTouching = nullptr;

	Vec2 mousePosition = g_theGame->GetMousePositionOnMainCamera();

	if( IsPointInsideDisc2D( mousePosition, entity->m_position, entity->m_physicsRadius ) )
	{
		m_entityMouseIsTouching = entity;
	}
}


TileMetaData& Map::GetTileMetaDataAtTilePosition( IntVec2 currentCoords )
{
	int tileIndex = GetTileIndexForTileCoordinates(currentCoords);
	return m_tileMetaData[tileIndex];
}

void Map::AddTagsAtPosition( const IntVec2& tileCoords, const Tags& tags )
{
	int tagIndex = GetTileIndexForTileCoordinates(tileCoords);
	m_tileMetaData[tagIndex].m_tags.AppendTags(tags);
}

Tile* Map::GetTileAtPosition( const IntVec2& tileCoords )
{
	if( !IsTilePositionValid( tileCoords ) )
	{
		return nullptr;
	}

	int tileIndex = GetTileIndexForTileCoordinates(tileCoords);
	Tile* tileToGet = &m_tiles[tileIndex];
	return tileToGet;
}


void Map::SetTileAtPosition( const IntVec2& tileCoords, TileDefinition* tileDef )
{
	if( !IsTilePositionValid( tileCoords ) )
	{
		return;
	}
	Tile* tile = GetTileAtPosition(tileCoords);
	tile->SetTileType(tileDef);
}


bool Map::IsTilePositionValid( IntVec2 mouseTilePosition )
{
	if( mouseTilePosition.x >= 0 && mouseTilePosition.x < m_mapSize.x &&
		mouseTilePosition.y >= 0 && mouseTilePosition.y < m_mapSize.y)
	{
		return true;
	}
	return false;
}


void Map::RenderDebugMode()
{ 
	AABB2 cameraBox = g_theGame->GetUICamera();
	AABB2 UIBottom = cameraBox.GetBoxAtBottom(0.2f);
	AABB2 UIBottomTop = UIBottom.CarveBoxOffBottom(0.5f);
	AABB2 UIBottomTopRight = UIBottomTop.GetBoxAtRight(0.3f);
	UIBottomTopRight.MakeSquareFromBottomLeft(true);
	AABB2 UIBOttomRight = UIBottom.GetBoxAtRight(0.3f);
	//AABB2 portraitBox = AABB2(11.f,4.f,12.f,5.f);

	Actor* actor = (Actor*)(m_entityMouseIsTouching);

	if( m_entityMouseIsTouching )
	{
		std::string stringToPrint = Stringf("Entity Name: %s, time until moving: %f", actor->m_actorDef->m_name.c_str(), actor->m_timeUntilNextGoalPosition);
		//std::string stringToPrint = Stringf("Entity Name: ");
		g_theRenderer->DrawAlignedTextAtPosition(stringToPrint.c_str(), UIBOttomRight, 1.f, ALIGN_CENTER_LEFT);

		const Texture& portraitTexture = g_portraitSpriteSheet->GetTexture();
		g_theRenderer->BindTexture(&portraitTexture);
		g_theRenderer->DrawAABB2Filled(UIBottomTopRight,Rgba8::WHITE,actor->m_actorDef->m_spritePortraitUVs.mins, actor->m_actorDef->m_spritePortraitUVs.maxs);
		return;
	}

	if( m_tileMouseIsTouching )
	{
		const char* tileName = m_tileMouseIsTouching->m_tileDef->m_name.c_str();
		std::string stringToPrint = Stringf("Tile Name: %s", tileName);
		g_theRenderer->DrawAlignedTextAtPosition(stringToPrint.c_str(), UIBOttomRight, 1.f, ALIGN_CENTER_LEFT);

		const Texture& tileTexture = g_tileSpriteSheet->GetTexture();
		g_theRenderer->BindTexture( &tileTexture );
		g_theRenderer->DrawAABB2Filled( UIBottomTopRight, Rgba8::WHITE, m_tileMouseIsTouching->m_tileDef->m_spriteUVs.mins, m_tileMouseIsTouching->m_tileDef->m_spriteUVs.maxs );
		return;
	}
}

void Map::UpdateDebugMode()
{
	UpdateEntityMouseIsTouching();
	UpdateTileMouseIsTouching();
}

int Map::GetTileIndexForTileCoordinates( const IntVec2& tileCoords )
{
	int tileIndex = tileCoords.x + m_mapSize.x * tileCoords.y;
	return tileIndex;
}
IntVec2 Map::GetTileCoordinatesForTileIndex( int tileIndex ) const
{
	return m_tiles[tileIndex].m_tileCoords;
}