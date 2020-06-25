#pragma once
#include "Game/Map.hpp"
#include "Engine/Core/EventSystem.hpp"
#include <map>

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

	bool WarpPlayer( EventArgs const& args );
	Entity* GetClosestEntityInSector( Vec3 const& position, Vec2 const& forwardVector, float forwardSpread, float maxDistance );

private:
	std::map<std::string, Map*> m_maps;

	Map* m_currentMap = nullptr;
	Game* m_game = nullptr;
public:
};