#pragma once
#include "Engine/Math/IntVec2.hpp"
#include <vector>
#include <string>

struct TileMetaData;
struct Tags;
class Tile;
class Entity;
class Game;
class Actor;
struct Vertex_PCU;
class MapDefinition;
class TileDefinition;

class Map
{
	friend class MapGenStep;
	friend class MapGenStep_Mutate;
	friend class MapGenStep_Worm;
	friend class MapGenStep_CellularAutomata;
public:
	Map(const char* mapDefName );
	Map() = delete;
	~Map();

	void Startup();
	void Shutdown();

	void Update( float deltaSeconds );
	void Render();
	void RenderDebugMode();

	int GetTileIndexForTileCoordinates( const IntVec2& tileCoords );
	IntVec2 GetTileCoordinatesForTileIndex( int tileIndex ) const;
	bool IsValidTileCoordinates( const IntVec2& tileCoords ) const;

	Actor* GetPlayer();
	void GetPlayers(std::vector<Actor*>& players);
	IntVec2 GetMapBounds() const;

	std::vector<Tile*> GetNeighboringTiles( Tile* currentTile );
	Tile* GetTileAtPosition( const IntVec2& tileCoords );
	void SetTileAtPosition( const IntVec2& tileCoords, TileDefinition* tileDef );
	TileMetaData& GetTileMetaDataAtTilePosition( IntVec2 currentCoords );

	void AddTagsAtPosition( const IntVec2& tileCoords, const Tags& tags);
private:
	Tile& GetRandomMapTile();
	Tile& GetRandomAdjacentMapTile(Tile& currentTile);

	void SpawnTiles();
	void SpawnEntities();
	void SpawnBullets();
	void SpawnBullet( Entity* shooter );

	void RenderTiles();
	void RenderEntities();

	void UpdateTiles( float deltaSeconds );
	void UpdateEntities( float deltaSeconds );
	void UpdateDebugMode();
	void UpdateEntityMouseIsTouching();
	void UpdateTileMouseIsTouching();

	void PushEntityOutOfWalls(Entity* currentEntity);
	void PushEntityOutOfWall( Entity* entityToPush, const Tile& tile );
	void PushEntitiesOutOfWalls();
	bool IsTilePositionValid( IntVec2 mouseTilePosition );

	void SetMouseIsTouchingEntity( Entity* entity );

	void GarbageCollectEntities();

protected:
	std::vector<Tile> m_tiles;
	std::vector<TileMetaData> m_tileMetaData;
	std::vector<Entity*> m_entities;
	std::vector<Vertex_PCU> m_vertsToRender;
	IntVec2 m_mapSize;
	MapDefinition* m_mapDef = nullptr;
	std::string m_name;

	//Debug
	Entity* m_entityMouseIsTouching = nullptr;
	Tile*	m_tileMouseIsTouching = nullptr;

};