#pragma once
#include "Game/Map.hpp"
#include "Game/GameCommon.hpp"

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

	void AddNewPlayer( int playerSlot );

	void CreateEntity( CreateEntityMessage const& createMessage );
	void UpdateEntity( UpdateEntityMessage const& updateMessage );
	void DeleteEntity( DeleteEntityMessage const& deleteMessage );

public:
	Map* m_currentMap = nullptr;
private:
	Game* m_game = nullptr;

	std::vector<Map*> m_maps;
	int m_currentMapIndex = 0;
public:
	IntVec2 GetCurrentMapBounds() const;
};