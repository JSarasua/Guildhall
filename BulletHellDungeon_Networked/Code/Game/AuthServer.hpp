#pragma once
#include "Game/Server.hpp"
#include <vector>

class Entity;
class SpriteDefinition;
class Game;

struct Vertex_PCU;

enum eGameState : int;

class AuthServer : public Server
{
public:
	AuthServer( Game* game );
	virtual ~AuthServer();

	virtual void Startup() override;
	virtual void Shutdown() override;
	virtual void BeginFrame() override;
	virtual void EndFrame() override;
	virtual void RestartGame() override;
	virtual void PauseGame() override;
	virtual void UnpauseGame() override;

	virtual void Update( float deltaSeconds ) override;
	virtual void UpdateGameState( eGameState newGamestate ) override;

	virtual int GetPlayerHealth() override;
	virtual int GetBossHealth() override;
	virtual SpriteDefinition const* GetPlayerWeaponSprite() const override;

	virtual std::vector<Vertex_PCU> const& GetTileVertsToRender() override;
	virtual std::vector<Entity*> const& GetEntitiesToRender() override;
	virtual eGameState GetCurrentGameState() override;
};