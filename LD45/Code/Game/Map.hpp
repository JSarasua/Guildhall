#pragma once
#include <vector>
#include "Engine/Math/IntVec2.hpp"


class Game;
class Tile;
class Entity;

class Map
{
public:
	Map();
	~Map();
	Map(int mapWidth, int mapHeight, int tileSize, Game* game);
	Map( const IntVec2& mapSize, int tileSize, Game* game );
	//Map( const IntVec2& mapSize, int tileSize);

	void Startup();
	void Shutdown();
	void RunFrame();

	void Update( float deltaSeconds );
	void Render();


private:
	std::vector<Tile> m_tiles;
	std::vector<Entity*> m_entities;
	IntVec2 m_mapSize;
	int m_NumOftiles = 0;
	int m_tileSize = 0;
	Game* m_game = nullptr;
	void CheckCollisions();
	void StartNextLevel();
	bool m_StartNextLevel = false;
	bool m_PlayerMustGoLeft = false;
	bool m_PlayerMustGoRight = false;
	float m_timer = 1.f;
};