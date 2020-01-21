#pragma once
#include <vector>
#include "Engine/Math/IntVec2.hpp"
#include "Game/Entity.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/RaycastResult.hpp"


enum Direction
{
	Left,
	Right,
	Up,
	Down
};


enum Level
{
	LEVELONE,
	LEVELTWO,
	LEVELTHREE,

	NUMOFLEVELS
};

class Tile;
class Game;
class Player;
struct Vertex_PCU;
class SpriteAnimDefinition;

class Map
{
public:
	Map();
	~Map();
	Map(const IntVec2& mapSize, Level level, Game* game);

	void Startup();
	void Startup(Player* currentPlayer, int currentLives);
	void Shutdown();

	void Update( float deltaSeconds );
	void Render();


	int GetTileIndexForTileCoordinates( const IntVec2& tileCoords ) const;
	IntVec2 GetTileCoordinatesForTileIndex( int tileIndex ) const;
	const TileDefinition& GetTileDefinition() const;

	Player* GetPlayer();
	bool ShouldGoToNextLevel() const;
	IntVec2 GetMapBounds() const;

	void SpawnNewEntity(EntityType type, EntityFaction faction, const Vec2& spawnPosition, float orientationDegrees, float size = 1.f);
	int GetCurrentLives();

	RaycastResult RayCast( const Vec2 startPos, const Vec2 endPos );
private:
	std::vector<Tile> m_tiles;
	std::vector<Entity*> m_entities;
	std::vector<Vertex_PCU> m_vertsToRender;
	std::vector<RaycastResult> m_raycastResults;
	EntityList m_EntityListsByType[NUM_OF_ENTITY_TYPES];
	SpriteAnimDefinition* m_explosionAnimDef = nullptr;

	TileDefinition m_tileDefinition;
	Texture* m_tileTexture;


	//std::vector<Entity*> m_EntityListsByType[NUM_OF_ENTITY_TYPES];
	Game* m_game = nullptr;
	IntVec2 m_mapSize;
	int m_NumOftiles = 0;
	int m_PlayerLives = 3;
	bool m_GoToNextLevel = false;
	Level m_level;

	void SetPlayer( Player* currentPlayer );
	void CheckIfPlayerExited();
	void CreateExplosionAnimation();
	IntVec2 GetRandomValidAdjacentTilePosition( const IntVec2& tilePosition ) const;
	void RebuildMap();
	bool ValidateTileMap( IntVec2 startPosition, IntVec2 exitPosition );

	void SpawnLevelOneTiles();
	void SpawnLevelTwoTiles();
	void SpawnLevelThreeTiles();
	void RebuildLevelOne();
	void RebuildLevelTwo();
	void RebuildLevelThree();
	void SpawnLevelOneEntities();
	void SpawnLevelTwoEntities();
	void SpawnLevelThreeEntities();
	const Vec2 GetRandomValidEntityPosition();
private:

	void CheckInput();
	void AddEntityToMap( Entity* newEntity );
	void AddEntityToList( Entity* newEntity, EntityList& myList );
	void UpdateEntitiesByList( EntityList& listToUpdate, float deltaSeconds );
	void RenderEntitiesByList( EntityList& listToRender );
	void ResolveEntityCollisionByList( EntityList& listToResolve );
	void ResolveEntityCollision( Entity* entity );
	void ResolveEntityWallCollision( Entity* entity );
	void ResolveEntityEntityCollision( Entity* entity );
	void ResolveSingleEntityCollision( Entity* entity, Entity* otherEntity );
	void CheckIfPlayerVisible();
	bool HasLineOfSight( Entity* entity );
	void RenderRaycasts();
	void CreateRaycasts( EntityList& listToUpdate );
	void UpdateEntityStates( EntityList& listToUpdate, EntityType entityType );
	void RenderTiles();
	void RenderEntities();
	void UpdateTiles( float deltaSeconds );
	void UpdateEntities( float deltaSeconds );
	void SpawnTiles();
	void SpawnEntities();
	void PushPlayerOutOfWalls();
	void GarbageCollectEntities();
	void GarbageCollectEntitiesByList( EntityList& listToGarbageCollect );



	bool IsPointInSector( const Vec2& point, const Vec2& startPos, float maxDist, const Vec2& fwdDir, float apertureDegrees);
	bool IsPointInSolid( const Vec2& point, Entity* entity = nullptr, AABB2* aabb = nullptr);
	bool IsTileSolid(int tileIndex) const;

	void SpawnWorm( TileType tileType, int wormLength );
	bool IsValidTileCoordinates( const IntVec2& tileCoords ) const;
	bool IsNonSolidTileCoordinates( const IntVec2& tilePosition ) const;

	int FloodFill( IntVec2 startPosition, int currentStep, std::vector<int>& validMapTiles  );
};