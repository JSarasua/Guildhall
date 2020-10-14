#pragma once
#include <vector>

class Entity;
class SpriteDefinition;

struct Vertex_PCU;

enum eGameState : int;

class Server
{
public:
	Server();
	~Server();
	virtual void Startup();
	virtual void Shutdown();
	virtual void BeginFrame();
	virtual void EndFrame();
	virtual void RestartGame();
	virtual void PauseGame();
	virtual void UnpauseGame();

	virtual void Update( float deltaSeconds );
	virtual void UpdateGameState( eGameState newGamestate );

	virtual int GetPlayerHealth();
	virtual int GetBossHealth();
	virtual SpriteDefinition const* GetPlayerWeaponSprite() const;

	virtual std::vector<Vertex_PCU> const& GetTileVertsToRender();
	virtual std::vector<Entity*> const& GetEntitiesToRender();
	virtual eGameState GetCurrentGameState();

};