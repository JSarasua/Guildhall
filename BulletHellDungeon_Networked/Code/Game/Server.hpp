#pragma once

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

};