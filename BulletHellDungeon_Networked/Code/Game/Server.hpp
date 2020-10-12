#pragma once
#include <vector>

struct Vertex_PCU;
class Entity;

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

	std::vector<Vertex_PCU> const& GetTileVertsToRender();
	std::vector<Entity*> const& GetEntitiesToRender();
	eGameState GetCurrentGameState();

};