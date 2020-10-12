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
	void Startup();
	void Shutdown();
	void BeginFrame();
	void EndFrame();
	void RestartGame();
	void PauseGame();
	void UnpauseGame();

	void Update( float deltaSeconds );
	void UpdateGameState( eGameState newGamestate );

	int GetPlayerHealth();
	int GetBossHealth();
	SpriteDefinition const* GetPlayerWeaponSprite() const;

	std::vector<Vertex_PCU> const& GetTileVertsToRender();
	std::vector<Entity*> const& GetEntitiesToRender();
	eGameState GetCurrentGameState();

};