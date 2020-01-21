#pragma once
#include <vector>
#include <string>
#include "Engine/Math/IntVec2.hpp"


class Tile;
class Entity;
class Game;
class Actor;
struct Vertex_PCU;
class MapDefinition;

class Map
{
public:
	Map(const char* mapDefName );
	Map() = delete;
	~Map();

	void Startup();
	void Shutdown();

	void Update( float deltaSeconds );
	void Render();

	int GetTileIndexForTileCoordinates( const IntVec2& tileCoords );
	IntVec2 GetTileCoordinatesForTileIndex( int tileIndex ) const;

	Actor* GetPlayer();

	IntVec2 GetMapBounds() const;

private:
	void RenderTiles();
	void RenderEntities();
	void UpdateTiles( float deltaSeconds );
	void UpdateEntities( float deltaSeconds );
	void SpawnTiles();
	void SpawnEntities();

	void PushEntityOutOfWalls(Entity* currentEntity);
	void PushEntityOutOfWall( Entity* entityToPush, const Tile& tile );
	void PushEntitiesOutOfWalls();

private:
	std::vector<Tile> m_tiles;
	std::vector<Entity*> m_entities;
	std::vector<Vertex_PCU> m_vertsToRender;
	IntVec2 m_mapSize;
	MapDefinition* m_mapDef = nullptr;
	std::string m_name;
};