#pragma once

class Client
{
public:
	Client();
	~Client();

	void Startup();
	void Shutdown();

	void BeginFrame();
	void EndFrame();

	void Update( float deltaSeconds );
	void Render();
};