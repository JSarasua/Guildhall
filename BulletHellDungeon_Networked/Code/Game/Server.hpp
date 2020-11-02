#pragma once
#include "Game/Client.hpp"
#include <vector>

class Entity;
class SpriteDefinition;
class Game;

struct Vertex_PCU;

enum eGameState : int;

class Server
{
public:
	Server( Game* game );
	virtual ~Server();
	virtual void Startup() = 0;
	virtual void Shutdown() = 0;
	virtual void BeginFrame() = 0;
	virtual void EndFrame() = 0;
	virtual void RestartGame() = 0;
	virtual void PauseGame() = 0;
	virtual void UnpauseGame() = 0;

	virtual void Update( float deltaSeconds ) = 0;
	virtual void UpdateGameState( eGameState newGamestate ) = 0;

	virtual int GetPlayerHealth() = 0;
	virtual int GetBossHealth() = 0;
	virtual SpriteDefinition const* GetPlayerWeaponSprite() const = 0;

	virtual std::vector<Vertex_PCU> const& GetTileVertsToRender() = 0;
	virtual std::vector<Entity*> const& GetEntitiesToRender() = 0;
	virtual eGameState GetCurrentGameState() = 0;

};