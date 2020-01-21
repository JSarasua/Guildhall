#pragma once
#include "Game/Map.hpp"
#include <vector>


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

	int GetCurrentLives();
	void TriggerNextMap();
	bool IsGameCompleted() const;
	Player* GetPlayer();

private:
	Map* m_currentMap = nullptr;
	std::vector<Map*> m_maps;
	int m_currentMapIndex = 0;
	Game* m_game = nullptr;
	bool m_GoToNextMap = false;
	bool m_isGameCompleted = false;

public:
	IntVec2 getCurrentMapBounds() const;

};