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

private:
	Map* m_currentMap = nullptr;
	Game* m_game = nullptr;
public:
};