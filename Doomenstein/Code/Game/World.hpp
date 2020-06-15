#pragma once
#include "Game/Map.hpp"


class Game;






class World
{
public:
	World(){}
	~World(){}

	World(Game* game);

	void Startup();
	void Shutdown();

	void Update( float deltaSeconds );
	void Render();

	void SetCurrentMap( Map* newCurrentMap );

private:
	std::vector<Map*> m_maps;

	Map* m_currentMap = nullptr;
	Game* m_game = nullptr;
public:
};