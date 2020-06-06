#include "Map.hpp"
#include "Tile.hpp"
#include "Entity.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Player.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/App.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Renderer/RenderContext.hpp"

extern App* g_theApp;
extern RenderContext* g_theRenderer;


Map::Map() : 
	m_mapSize(0,0)
{}

Map::Map(const IntVec2& mapSize, Game* game) :
	m_mapSize(mapSize),
	m_game(game)
{
	m_NumOftiles = m_mapSize.x * m_mapSize.y;
	SpawnTiles();
	SpawnEntities();

	//m_vertsToRender.reserve(m_tiles.size());
}

Map::~Map()
{

}


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
	g_theRenderer->SetModelMatrix( Mat44() );
	g_theRenderer->SetBlendMode( eBlendMode::ALPHA );
	g_theRenderer->SetDepth( eDepthCompareMode::COMPARE_ALWAYS, eDepthWriteMode::WRITE_ALL );
	RenderTiles();
	RenderEntities();

}

Player* Map::GetPlayer()
{
	return static_cast<Player*>(m_entities[0]);
}

IntVec2 Map::GetMapBounds() const
{
	return m_mapSize;
}

void Map::RenderTiles()
{
	g_theRenderer->BindTexture(nullptr);
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
		PushPlayerOutOfWalls();
	}

}

void Map::SpawnTiles()
{
	int x = 0;
	int y = 0;


	for( int mapIndex = 0; mapIndex < m_NumOftiles; mapIndex++ )
	{
		y = mapIndex / (m_mapSize.x);
		x = mapIndex - y * m_mapSize.x;

			m_tiles.push_back( Tile( IntVec2( x, y ), GRASS, mapIndex) );

	}

	//Randomize solid tiles
	int randomNum = m_game->m_rand.RollRandomIntInRange(0,10);
	
	for( int mapIndex = 0; mapIndex < m_tiles.size(); mapIndex++ )
	{
		randomNum = m_game->m_rand.RollRandomIntInRange(0,10);
		if (randomNum > 7)
		{
			m_tiles[mapIndex].m_tileType = STONE;
		}
	}

	//Create Safe zone
	for( int safeZoneIndexX = 1; safeZoneIndexX < 6; safeZoneIndexX++ )
	{
		for( int safeZoneIndexY = 1; safeZoneIndexY < 6; safeZoneIndexY++ )
		{
			m_tiles[GetTileIndexForTileCoordinates(IntVec2(safeZoneIndexX, safeZoneIndexY))].m_tileType = GRASS;
		}
	}
	for( int safeZoneIndexX = INT_MAP_SIZE_X - 1; safeZoneIndexX > INT_MAP_SIZE_X - 7; safeZoneIndexX-- )
	{
		for( int safeZoneIndexY = INT_MAP_SIZE_Y - 1; safeZoneIndexY > INT_MAP_SIZE_Y - 7; safeZoneIndexY-- )
		{
			m_tiles[GetTileIndexForTileCoordinates( IntVec2( safeZoneIndexX, safeZoneIndexY ) )].m_tileType = GRASS;
		}
	}


	//Border wall
	int tileIndex = 0;
	for( int borderXIndex= 0; borderXIndex< INT_MAP_SIZE_X; borderXIndex++ )
	{
		m_tiles[borderXIndex].m_tileType = STONE;

		tileIndex = INT_MAP_SIZE_X * (INT_MAP_SIZE_Y - 1) + borderXIndex;
		m_tiles[tileIndex].m_tileType = STONE;
	}
	for( int borderYIndex= 0; borderYIndex < INT_MAP_SIZE_Y; borderYIndex++ )
	{
		tileIndex = borderYIndex * INT_MAP_SIZE_X;
		m_tiles[tileIndex].m_tileType = STONE;

		tileIndex = borderYIndex * INT_MAP_SIZE_X + (INT_MAP_SIZE_X-1);
		m_tiles[tileIndex].m_tileType = STONE;
	}

}

void Map::SpawnEntities()
{
	m_entities.push_back( new Player( Vec2(2.5f, 2.5f), m_game ) );

}


void Map::PushPlayerOutOfWalls()
{
	IntVec2 playerPostionInt(m_entities[0]->GetPosition());
	Vec2 playerPosition(m_entities[0]->GetPosition());

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
	m_entities[0]->SetPosition(playerPosition);
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

