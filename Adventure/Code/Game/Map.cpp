#include "Map.hpp"
#include "Tile.hpp"
#include "Entity.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/App.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Game/MapDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Game/TileDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Game/Actor.hpp"
#include "Game/ActorDefinition.hpp"

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
}

void Map::Render()
{
	RenderTiles();
	RenderEntities();

}

Actor* Map::GetPlayer()
{
	return static_cast<Actor*>(m_entities[0]);
}

IntVec2 Map::GetMapBounds() const
{
	return m_mapSize;
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

			//m_tiles.push_back( Tile( IntVec2( x, y ), GRASS) );
		m_tiles.push_back(Tile(IntVec2(x,y), fillTileDef));

	}


	//Randomize solid tiles
	int randomNum = g_theGame->m_rand.RollRandomIntInRange( 0, 10 );

	for( int mapIndex = 0; mapIndex < m_tiles.size(); mapIndex++ )
	{
		randomNum = g_theGame->m_rand.RollRandomIntInRange( 0, 10 );
		if( randomNum > 7 )
		{
			m_tiles[mapIndex].SetTileType(edgeTileDef);
		}
	}

	//Create Safe zone
	for( int safeZoneIndexX = 1; safeZoneIndexX < 6; safeZoneIndexX++ )
	{
		for( int safeZoneIndexY = 1; safeZoneIndexY < 6; safeZoneIndexY++ )
		{
			int tileIndex = GetTileIndexForTileCoordinates( IntVec2( safeZoneIndexX, safeZoneIndexY ) );
			m_tiles[tileIndex].SetTileType(fillTileDef);
		}
	}
	for( int safeZoneIndexX = m_mapSize.x - 1; safeZoneIndexX > m_mapSize.x - 7; safeZoneIndexX-- )
	{
		for( int safeZoneIndexY = m_mapSize.y - 1; safeZoneIndexY > m_mapSize.y - 7; safeZoneIndexY-- )
		{
			int tileIndex = GetTileIndexForTileCoordinates( IntVec2( safeZoneIndexX, safeZoneIndexY ) );
			m_tiles[tileIndex].SetTileType( fillTileDef );
		}
	}





	//Border wall
	int tileIndex = 0;
	for( int borderXIndex= 0; borderXIndex < m_mapSize.x; borderXIndex++ )
	{
		//m_tiles[borderXIndex].m_tileType = STONE;
		m_tiles[borderXIndex].SetTileType(edgeTileDef);

		tileIndex = m_mapSize.x * (m_mapSize.y - 1) + borderXIndex;
		//m_tiles[tileIndex].m_tileType = STONE;
		m_tiles[tileIndex].SetTileType(edgeTileDef);
	}
	for( int borderYIndex= 0; borderYIndex < m_mapSize.y; borderYIndex++ )
	{
		tileIndex = borderYIndex * m_mapSize.x;
		//m_tiles[tileIndex].m_tileType = STONE;
		m_tiles[tileIndex].SetTileType(edgeTileDef);

		tileIndex = borderYIndex * m_mapSize.x + (m_mapSize.x-1);
		//m_tiles[tileIndex].m_tileType = STONE;
		m_tiles[tileIndex].SetTileType(edgeTileDef);
	}

}

void Map::SpawnEntities()
{
	ActorDefinition* playerActorDef = ActorDefinition::s_definitions["Player"];
	//m_entities.push_back( new Player( Vec2(2.5f, 2.5f) ) );
	m_entities.push_back( new Actor(Vec2(2.5, 2.5f),Vec2(0.f,0.f), 0.f, 0.f, playerActorDef));

}


void Map::PushEntityOutOfWalls(Entity* currentEntity)
{
	IntVec2 entityPostionInt(currentEntity->GetPosition());
	Vec2 entityPosition(currentEntity->GetPosition());
	//Entity* currentEntity = m_entities[0];
	//float physicsRadius = currentActor->m_actorDef->m_physicsRadius;

	//Grab all of the tile coordinates for tiles around the player
	IntVec2 leftPlayerPositionInt(entityPostionInt.x -1,entityPostionInt.y);
	IntVec2 rightPlayerPositionInt(entityPostionInt.x + 1, entityPostionInt.y);
	IntVec2 upPlayerPositionInt(entityPostionInt.x, entityPostionInt.y + 1);
	IntVec2 downPlayerPositionInt(entityPostionInt.x, entityPostionInt.y - 1);
	IntVec2 topLeftPlayerPositionInt( entityPostionInt.x -1, entityPostionInt.y + 1 );
	IntVec2 topRightPlayerPositionInt( entityPostionInt.x + 1, entityPostionInt.y + 1 );
	IntVec2 botttomLeftPlayerPositionInt( entityPostionInt.x - 1, entityPostionInt.y - 1 );
	IntVec2 bottomRightPlayerPositionInt( entityPostionInt.x + 1, entityPostionInt.y - 1 );


	const Tile& leftTile = m_tiles[GetTileIndexForTileCoordinates( leftPlayerPositionInt )];
	const Tile& rightTile = m_tiles[GetTileIndexForTileCoordinates( rightPlayerPositionInt)];
	const Tile& upTile = m_tiles[GetTileIndexForTileCoordinates( upPlayerPositionInt )];
	const Tile& downTile = m_tiles[GetTileIndexForTileCoordinates( downPlayerPositionInt )];
	const Tile& topLeftTile = m_tiles[GetTileIndexForTileCoordinates( topLeftPlayerPositionInt )];
	const Tile& topRightTile = m_tiles[GetTileIndexForTileCoordinates( topRightPlayerPositionInt )];
	const Tile& bottomLeftTile = m_tiles[GetTileIndexForTileCoordinates( botttomLeftPlayerPositionInt )];
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
	entityToPush->SetPosition(entityPosition);

}

void Map::PushEntitiesOutOfWalls()
{
	for( int entityIndex = 0; entityIndex < m_entities.size(); entityIndex++ )
	{
		PushEntityOutOfWalls(m_entities[entityIndex]);
	}
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

