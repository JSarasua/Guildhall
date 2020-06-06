#pragma once
#include <vector>
#include "Engine/Math/IntVec2.hpp"


class Tile;
class Entity;
class Game;
class Player;
struct Vertex_PCU;

class Map
{
public:
	Map();
	~Map();
	Map(const IntVec2& mapSize, Game* game);

	void Startup();
	void Shutdown();

	void Update( float deltaSeconds );
	void Render();

	int GetTileIndexForTileCoordinates( const IntVec2& tileCoords );
	IntVec2 GetTileCoordinatesForTileIndex( int tileIndex ) const;

	Player* GetPlayer();

	IntVec2 GetMapBounds() const;

private:
	std::vector<Tile> m_tiles;
	std::vector<Entity*> m_entities;
	std::vector<Vertex_PCU> m_vertsToRender;
	Game* m_game = nullptr;
	IntVec2 m_mapSize;
	int m_NumOftiles = 0;
	void RenderTiles();
	void RenderEntities();
	void UpdateTiles( float deltaSeconds );
	void UpdateEntities( float deltaSeconds );
	void SpawnTiles();
	void SpawnEntities();

	void PushPlayerOutOfWalls();
};