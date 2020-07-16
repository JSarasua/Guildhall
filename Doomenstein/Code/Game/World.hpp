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
	void WarpPlayerWithEntity( Entity* entityToWarp, std::string const& destMap, Vec2 const& destPos, float destYawOffset );
	Entity* GetClosestEntityInSector( Vec3 const& position, Vec2 const& forwardVector, float forwardSpread, float maxDistance );
	Map* GetMapByName( std::string const& mapName );


private:
	std::map<std::string, Map*> m_maps;

	Map* m_currentMap = nullptr;
	Game* m_game = nullptr;
public:
};