#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/vec2.hpp"
#include <vector>
#include <string>
#include "Game/GameCommon.hpp"

struct TileMetaData;
struct Tags;
struct Vertex_PCU;
class Tile;
class Entity;
class Game;
class Actor;
class MapDefinition;
class TileDefinition;
class EnemySpawner;

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

	std::vector<Vertex_PCU> const& GetTileVertsToRender() { return m_vertsToRender; }
	std::vector<Entity*> const& GetEntitiesToRender() { return m_entities; }

	int GetTileIndexForTileCoordinates( const IntVec2& tileCoords );
	IntVec2 GetTileCoordinatesForTileIndex( int tileIndex ) const;
	bool IsValidTileCoordinates( const IntVec2& tileCoords ) const;
	bool IsTileSolid( IntVec2 const& tileCoords );

	Actor* GetPlayer();
	void GetPlayers(std::vector<Actor*>& players);
	IntVec2 GetMapBounds() const;

	std::vector<Tile*> GetNeighboringTiles( Tile* currentTile );
	Tile* GetTileAtPosition( const IntVec2& tileCoords );
	void SetTileAtPosition( const IntVec2& tileCoords, TileDefinition* tileDef );
	TileMetaData& GetTileMetaDataAtTilePosition( IntVec2 currentCoords );

	void AddTagsAtPosition( const IntVec2& tileCoords, const Tags& tags);

	Entity* GetEntityWithID( int entityID ) const;
	void SpawnEntity( Entity* entityToSpawn );
	void UpdateEntity( UpdateEntityMessage const& updateMessage );
	void DeleteEntity( DeleteEntityMessage const& deleteMessage );
	void AddPlayer( Actor* player, int playerSlot = 0 );
	void DeletePlayer();

	bool IsBossDead() const;
	int GetBossHealth() const;

	void SpawnSpawnersLevel1();
	void SpawnSpawnersLevel2();
	void SpawnSpawnersLevel3();

private:
	Tile& GetRandomMapTile();
	Tile& GetRandomAdjacentMapTile(Tile& currentTile);

	void SpawnTiles();
	void SpawnEntities();
	void SpawnBullets();
	void SpawnBullet( Entity* shooter );
	void SpawnBossBullets( Actor* boss );

	void RenderTiles();
	void RenderEntities();

	void UpdateTiles( float deltaSeconds );
	void UpdateEntities( float deltaSeconds );
	void UpdateSpawners();
	void UpdateDebugMode();
	void UpdateEntityMouseIsTouching();
	void UpdateTileMouseIsTouching();

	void PushEntityOutOfWalls(Entity* currentEntity);
	void PushEntityOutOfWall( Entity* entityToPush, const Tile& tile );
	void PushEntitiesOutOfWalls();
	void ResolveEntitiesCollisions();
	void ResolveEntityCollisions( Entity* currentEntity );
	bool AreEntitiesColliding( Entity* entityA, Entity* entityB );
	bool IsTilePositionValid( IntVec2 mouseTilePosition );

	void SetMouseIsTouchingEntity( Entity* entity );

	void GarbageCollectEntities();



protected:
	std::vector<Tile> m_tiles;
	std::vector<TileMetaData> m_tileMetaData;
	std::vector<Entity*> m_entities;
	std::vector<Vertex_PCU> m_vertsToRender;
	std::vector<EnemySpawner*> m_enemySpawners;

	IntVec2 m_mapSize;
	MapDefinition* m_mapDef = nullptr;
	std::string m_name;
	Vec2 m_startPosition;

	//Debug
	Entity* m_entityMouseIsTouching = nullptr;
	Tile*	m_tileMouseIsTouching = nullptr;

};