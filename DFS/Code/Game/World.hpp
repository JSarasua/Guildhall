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
	void RenderDebug() const;
	void MoveToNextMap();

	Actor* GetPlayer();
	void GetPlayers(std::vector<Actor*>& players);
	int GetBossHealth() const;

private:
	Map* m_currentMap = nullptr;
	Game* m_game = nullptr;

	std::vector<Map*> m_maps;
	int m_currentMapIndex = 0;
public:
	IntVec2 getCurrentMapBounds() const;
};