#pragma once
#include <vector>
#include "Engine/Math/IntVec2.hpp"
#include "Game/Tile.hpp"

class Game;
class Entity;
class Player3D;
class Companion3D;
class Tile;

struct Vertex_PCU;
struct Vertex_PCUTBN;

class Map3D
{
public:
	Map3D();
	~Map3D();
	Map3D( IntVec2 const& mapSize, Game* game );

	void Startup();
	void Shutdown();

	void Update( float deltaSeconds );
	void Render();

private:
	void RenderTiles();
	void RenderEntities();

	void UpdateTiles( float deltaSeconds );
	void UpdateEntities( float deltaSeconds );

	void AddGravity( float deltaSeconds );


	void SpawnTiles();
	void SpawnEntities();

	void CheckForCollisions();

public:
	Player3D* m_player = nullptr;

private:
	Companion3D* m_companion = nullptr;
	std::vector<Tile> m_tiles;

	Game* m_game;
	IntVec2 m_mapSize;

};